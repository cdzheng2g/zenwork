#pragma once
#include <QFileInfo>
#include <QDateTime>
#include <list>
#include <memory>
#include <stdexcept>

// Thread-confined LRU. File-byte budget is a bound on model files, not SDK workspace memory.
template<class Session> class ModelSessionCache {
public:
    explicit ModelSessionCache(size_t maximum = 4, qint64 bytes = 256 * 1024 * 1024)
        : _maximum(maximum), _budget(bytes) {}
    std::shared_ptr<Session> acquire(const QString& path)
    {
        const QFileInfo info(path);
        if (!info.isFile() || info.size() <= 0 || info.size() > _budget)
            throw std::runtime_error("模型不存在、为空或超过缓存容量");
        const auto key = info.canonicalFilePath();
        for (auto it = _entries.begin(); it != _entries.end(); ++it) {
            if (it->path != key) continue;
            if (it->bytes == info.size() && it->modified == info.lastModified().toMSecsSinceEpoch()) {
                it->session->load(key); // also preserves the session's fault guard
                auto result = it->session;
                _entries.splice(_entries.begin(), _entries, it);
                return result;
            }
            if (it->session.use_count() != 1)
                throw std::runtime_error("模型仍被当前任务使用，不能替换");
            _bytes -= it->bytes;
            _entries.erase(it);
            break;
        }
        while (_entries.size() >= _maximum || _bytes + info.size() > _budget) {
            auto candidate = _entries.end();
            for (auto it = _entries.begin(); it != _entries.end(); ++it)
                if (it->session.use_count() == 1) candidate = it;
            if (candidate == _entries.end())
                throw std::runtime_error("模型缓存已满且正在使用，请减少同时使用的模型");
            _bytes -= candidate->bytes;
            _entries.erase(candidate);
        }
        auto session = std::make_shared<Session>();
        session->load(key);
        _entries.push_front({key, info.size(), info.lastModified().toMSecsSinceEpoch(), session});
        _bytes += info.size();
        return session;
    }
    size_t size() const { return _entries.size(); }
    qint64 modelBytes() const { return _bytes; }
private:
    struct Entry { QString path; qint64 bytes, modified; std::shared_ptr<Session> session; };
    size_t _maximum;
    qint64 _budget, _bytes {0};
    std::list<Entry> _entries;
};
