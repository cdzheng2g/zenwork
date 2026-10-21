#pragma once
#include <QString>

// Keys refer to saved operator parameters, so existing schemes keep their format.
struct ImageInputSpec {
    QString fullImageKey { "fullImage" };
    QString roiKey { "roi" };
    QString alignedKey;
    bool fullImageDefault { false };
    bool alignedDefault { true };
    bool fallbackToReference { true };
};
