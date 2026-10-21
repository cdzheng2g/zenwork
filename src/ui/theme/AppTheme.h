#pragma once
#include <QString>
namespace AppTheme {
inline constexpr int ControlHeight = 38;
inline constexpr int FormSpacing = 12;
inline QString controls()
{
    return QStringLiteral(
        "QDoubleSpinBox,QSpinBox,QLineEdit,QComboBox{background:#fbfcfd;color:#253041;border:1px solid "
        "#dce2e8;border-radius:8px;padding:4px 8px;font-size:14px;}"
        "QDoubleSpinBox:focus,QSpinBox:focus,QLineEdit:focus,QComboBox:focus{border:1px solid #b51d32;}"
        "QDoubleSpinBox:disabled,QSpinBox:disabled,QLineEdit:disabled,QComboBox:disabled{background:#f2f4f6;"
        "color:#9aa4b1;}"
        "QSlider::groove:horizontal{height:6px;background:#e5e9ee;border-radius:3px;}"
        "QSlider::sub-page:horizontal{background:#e45765;border-radius:3px;}"
        "QSlider::handle:horizontal{width:19px;margin:-7px 0;background:white;border:3px solid "
        "#b51d32;border-radius:9px;}"
        "QSlider::sub-page:horizontal:disabled{background:#d4d9df;}"
        "QSlider::handle:horizontal:disabled{border-color:#bec6cf;}"
        "QTabBar::tab{padding:10px 18px;} QTabBar::tab:selected{color:#b51d32;border-bottom:2px solid "
        "#b51d32;}");
}
}
