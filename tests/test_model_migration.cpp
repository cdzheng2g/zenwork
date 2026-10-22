#include "test_support.h"
#include "infrastructure/inference/rknn/ModelSessionCache.h"
#include "application/scheme/ToolCodec.h"
#include "ui/components/ParameterForm.h"
#include <QApplication>
#include <QComboBox>
#include <QFile>
#include <QTemporaryDir>
namespace {
struct FakeSession {
    QString loaded;
    static int loads;
    void load(const QString& path) { if (loaded != path) { loaded = path; ++loads; } }
};
int FakeSession::loads = 0;
bool throws(const std::function<void()>& call) {
    try { call(); return false; } catch (const std::exception&) { return true; }
}
}
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    Checks check;
    QTemporaryDir dir;
    auto file = [&](const char* name, int bytes) {
        auto path = dir.filePath(name);
        QFile f(path); f.open(QIODevice::WriteOnly); f.write(QByteArray(bytes, 'm')); f.close();
        return path;
    };
    const auto a = file("a.rknn", 10), b = file("b.rknn", 10), c = file("c.rknn", 10);
    ModelSessionCache<FakeSession> cache(2, 20);
    const auto first = cache.acquire(a).get();
    cache.acquire(b);
    for (int i=0; i<10; ++i) { cache.acquire(a); cache.acquire(b); }
    check(FakeSession::loads == 2 && cache.acquire(a).get() == first && cache.size() == 2,
        "alternating A/B models reuse two sessions instead of reloading each operator");
    auto pinnedA = cache.acquire(a), pinnedB = cache.acquire(b);
    check(throws([&] { cache.acquire(c); }), "pinned contexts cannot be evicted during inference");
    pinnedB.reset();
    cache.acquire(c);
    check(cache.size() == 2 && cache.modelBytes() == 20 && cache.acquire(a) == pinnedA,
        "LRU evicts only unused sessions and keeps count and file-byte budgets bounded");
    const auto huge = file("huge.rknn", 21);
    check(throws([&] { cache.acquire(huge); }), "oversized model fails before loading an SDK context");
    QFile changed(a); changed.open(QIODevice::Append); changed.write("x"); changed.close();
    check(throws([&] { cache.acquire(a); }), "model replacement is rejected while its context is borrowed");
    pinnedA.reset();
    cache.acquire(a);
    check(cache.modelBytes() <= 20, "modified model is reloaded after releasing its previous borrow");

    auto detection = OperatorRegistry::makeTool("detection");
    auto legacy = ToolCodec::encode(detection);
    legacy["version"] = 1;
    auto parameters = legacy["parameters"].toObject();
    parameters["resizeMode"] = "拉伸"; legacy["parameters"] = parameters;
    const auto migrated = ToolCodec::decode(legacy);
    check(migrated.version == 2 && migrated.parameters["resizeMode"] == "stretch",
        "legacy v1 Chinese resize parameter migrates to v2 stable enum value");
    check(ToolCodec::decode(ToolCodec::encode(migrated)).parameters == migrated.parameters,
        "migrated configuration round-trips without losing other parameters");
    legacy["version"] = 1.5;
    check(throws([&] { ToolCodec::decode(legacy); }), "fractional versions are rejected");
    legacy["version"] = 999;
    check(throws([&] { ToolCodec::decode(legacy); }), "unknown future version fails explicitly");
    const auto spec = choiceParameter("resizeMode", "缩放", "letterbox", {"letterbox", "stretch"},
        {{"letterbox", "等比例填充"}, {"stretch", "拉伸"}});
    ParameterForm form({spec});
    form.setConfiguration({{"resizeMode", "stretch"}});
    auto* combo = form.findChild<QComboBox*>();
    check(combo && combo->currentText() == "拉伸" && form.configuration()["resizeMode"] == "stretch",
        "UI uses translated labels while persisted choices retain stable values");
    return check.result();
}
