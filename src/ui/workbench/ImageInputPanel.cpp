#include "ImageInputPanel.h"

#include "ElaComboBox.h"
#include "ElaPushButton.h"
#include "ElaText.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QTimer>
#include <QVBoxLayout>

ImageInputPanel::ImageInputPanel(QWidget* parent)
    : QWidget(parent)
{
    initContent();
    _playbackTimer = new QTimer(this);
    _playbackTimer->setInterval(500);
    connect(_playbackTimer, &QTimer::timeout, this, &ImageInputPanel::playbackTick);
    updateControls();
}

ImageInputPanel::~ImageInputPanel()
{
    if (_playbackTimer)
        _playbackTimer->stop();
}

bool ImageInputPanel::isCameraSource() const
{
    return _sourceComboBox->currentIndex() == 0;
}

void ImageInputPanel::initContent()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* sourceLabel = new ElaText(QStringLiteral("输入源"), this);
    sourceLabel->setTextPixelSize(13);
    layout->addWidget(sourceLabel);

    _sourceComboBox = new ElaComboBox(this);
    _sourceComboBox->addItems({ QStringLiteral("实时相机"), QStringLiteral("图片文件夹") });
    _sourceComboBox->setFixedSize(130, 36);
    connect(_sourceComboBox, qOverload<int>(&ElaComboBox::currentIndexChanged), this,
        &ImageInputPanel::onSourceChanged);
    layout->addWidget(_sourceComboBox);

    _directoryButton = new ElaPushButton(QStringLiteral("选择文件夹"), this);
    _directoryButton->setFixedHeight(36);
    connect(_directoryButton, &ElaPushButton::clicked, this, &ImageInputPanel::selectDirectory);
    layout->addWidget(_directoryButton);

    _previousButton = new ElaPushButton(QStringLiteral("上一张"), this);
    _previousButton->setFixedHeight(36);
    connect(_previousButton, &ElaPushButton::clicked, this, &ImageInputPanel::showPrevious);
    layout->addWidget(_previousButton);

    _nextButton = new ElaPushButton(QStringLiteral("下一张"), this);
    _nextButton->setFixedHeight(36);
    connect(_nextButton, &ElaPushButton::clicked, this, &ImageInputPanel::showNext);
    layout->addWidget(_nextButton);

    _playButton = new ElaPushButton(QStringLiteral("自动播放"), this);
    _playButton->setFixedHeight(36);
    connect(_playButton, &ElaPushButton::clicked, this, &ImageInputPanel::togglePlayback);
    layout->addWidget(_playButton);

    _directoryText = new ElaText(this);
    _directoryText->setTextPixelSize(11);
    _directoryText->setIsWrapAnywhere(true);
    layout->addWidget(_directoryText, 1);

    _fileText = new ElaText(this);
    _fileText->setTextPixelSize(11);
    layout->addWidget(_fileText);

    _statusText = new ElaText(this);
    _statusText->setTextPixelSize(11);
    layout->addWidget(_statusText);
}

void ImageInputPanel::onSourceChanged(int index)
{
    if (index == 0) {
        if (_playbackTimer->isActive())
            _playbackTimer->stop();
        _playButton->setText(QStringLiteral("自动播放"));
        setStatus(QStringLiteral("已选择实时相机"));
        emit cameraSourceSelected();
    } else {
        if (_playbackTimer->isActive())
            _playbackTimer->stop();
        _playButton->setText(QStringLiteral("自动播放"));
        setStatus(
            _sequence.isEmpty() ? QStringLiteral("请选择图片文件夹") : QStringLiteral("图片文件夹已就绪"));
        emit folderSourceSelected();
    }
    updateControls();
}

void ImageInputPanel::selectDirectory()
{
    const QString directory = QFileDialog::getExistingDirectory(this, QStringLiteral("选择测试图片文件夹"));
    if (directory.isEmpty())
        return;

    QString error;
    if (!_sequence.setDirectory(directory, &error)) {
        setStatus(error, true);
        updateControls();
        return;
    }

    _sourceComboBox->setCurrentIndex(1);
    _directoryText->setText(directory);
    loadCurrentImage();
    updateControls();
}

void ImageInputPanel::showPrevious()
{
    if (_sequence.previous())
        loadCurrentImage();
    updateControls();
}

void ImageInputPanel::showNext()
{
    if (_sequence.next())
        loadCurrentImage();
    else if (_playbackTimer->isActive()) {
        _playbackTimer->stop();
        _playButton->setText(QStringLiteral("自动播放"));
        setStatus(QStringLiteral("已播放到最后一张"));
    }
    updateControls();
}

void ImageInputPanel::togglePlayback()
{
    if (_sequence.isEmpty()) {
        setStatus(QStringLiteral("请先选择包含图片的文件夹"), true);
        return;
    }

    if (_playbackTimer->isActive()) {
        _playbackTimer->stop();
        _playButton->setText(QStringLiteral("自动播放"));
        setStatus(QStringLiteral("自动播放已停止"));
    } else {
        _playbackTimer->start();
        _playButton->setText(QStringLiteral("停止播放"));
        setStatus(QStringLiteral("正在自动播放"));
    }
}

void ImageInputPanel::playbackTick()
{
    showNext();
}

void ImageInputPanel::loadCurrentImage()
{
    QString error;
    const cv::Mat image = _sequence.loadCurrent(&error);
    if (image.empty()) {
        setStatus(error, true);
        return;
    }

    _fileText->setText(QStringLiteral("%1/%2  %3")
            .arg(_sequence.index() + 1)
            .arg(_sequence.count())
            .arg(_sequence.currentFileName()));
    setStatus(QStringLiteral("图片已载入"));
    emit imageReady(image, _sequence.currentFileName());
}

void ImageInputPanel::updateControls()
{
    const bool folderMode = !isCameraSource();
    const bool hasImages = folderMode && !_sequence.isEmpty();
    _directoryButton->setVisible(folderMode);
    _previousButton->setVisible(folderMode);
    _nextButton->setVisible(folderMode);
    _playButton->setVisible(folderMode);
    _directoryText->setVisible(folderMode);
    _fileText->setVisible(folderMode);
    _previousButton->setEnabled(hasImages && _sequence.hasPrevious());
    _nextButton->setEnabled(hasImages && _sequence.hasNext());
    _playButton->setEnabled(hasImages);
}

void ImageInputPanel::setStatus(const QString& status, bool error)
{
    _statusText->setText(status);
    _statusText->setStyleSheet(error ? QStringLiteral("color:#ef6b73;") : QStringLiteral("color:#71d58a;"));
    emit sourceStatusChanged(status);
}
