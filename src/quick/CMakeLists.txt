ecm_add_qml_module(KEmojiPlugin URI org.kde.kemoji GENERATE_PLUGIN_SOURCE
    QML_FILES
        EmojiDelegate.qml
        EmojiGrid.qml
        SubEmojiPopup.qml
        CategoryAction.qml
        ToneAction.qml
    SOURCES
        quickdict.h
        categoryhelper.h
        tonehelper.h
)

target_link_libraries(KEmojiPlugin PRIVATE
    Qt::Qml
    KF6::I18n
    KEmoji
)

qt_generate_foreign_qml_types(KEmoji KEmojiPlugin)

ecm_finalize_qml_module(KEmojiPlugin DESTINATION ${KDE_INSTALL_QMLDIR})
