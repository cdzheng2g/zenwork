#include "infrastructure/reference/ReferenceImageManager.h"

#include <opencv2/core.hpp>

#include <cstdio>

namespace {
int failures = 0;
void check(bool value, const char* description)
{
    std::printf("[%s] %s\n", value ? "PASS" : "FAIL", description);
    if (!value)
        ++failures;
}
}

int main()
{
    check(!ReferenceImageManager::isValidModelName(QStringLiteral("../bad")),
        "a model name cannot escape its reference directory");
    check(!ReferenceImageManager::isValidModelName(QStringLiteral("line/a")),
        "a model name cannot contain a path separator");
    check(ReferenceImageManager::isValidModelName(QStringLiteral("瓶盖A型")),
        "a normal model name remains valid");
    check(!ReferenceImageManager::instance().importReferenceImage(QStringLiteral("空实时图"), cv::Mat()),
        "an empty live frame fails cleanly instead of terminating the application");
    return failures == 0 ? 0 : 1;
}
