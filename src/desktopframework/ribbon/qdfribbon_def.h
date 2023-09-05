#ifndef DESKTOPFRAMEWORK_QDFRIBBON_DEF_H
#define DESKTOPFRAMEWORK_QDFRIBBON_DEF_H

#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

extern const char *RibbonCustomizeQuickAccessToolBarDotString;
extern const char *RibbonCustomizeQuickAccessToolBarString;
extern const char *RibbonShowQuickAccessToolBarBelowString;
extern const char *RibbonShowQuickAccessToolBarAboveString;
extern const char *RibbonCustomizeDialogOptionsString;
extern const char *RibbonCustomizeActionString;
extern const char *RibbonMinimizeActionString;
extern const char *RibbonRecentDocumentsString;
extern const char *RibbonUntitledString;
extern const char *RibbonSeparatorString;
extern const char *RibbonNewPageString;
extern const char *RibbonNewGroupString;
extern const char *RibbonAddCommandWarningString;

#define _qdf_Index "index"
#define _qdf_Wrap "Wrap"
#define _qdf_Wrap_Group "WrapGroup"

#define _qdf_Begin_Group "BeginToolBarGroup"
#define _qdf_Group "ToolBarGroup"
#define _qdf_End_Group "EndToolBarGroup"

#define _qdf_Index_Group "IndexGroup"
#define _qdf_Switch_Count "SwitchCount"
#define _qdf_TitleGroupsVisible "TitleGroupsVisible"

#define _qdf_KeyTip "Qdf::KeyTip"

#define _qdf_PopupButtonGallery "PopupButtonGallery"
#define _qdf_ScrollUpButtonGallery "ScrollUpButtonGallery"
#define _qdf_ScrollDownButtonGallery "ScrollDownButtonGallery"

#define _qdf_PopupBar "PopupBar"
#define _qdf_PopupLable "PopupLable_"
#define _qdf_SplitActionPopup "SplitActionPopup_"
#define _qdf_WidgetGallery "WidgetGallery"
#define _qdf_StyleName "StyleName"

#define __qdf_Widget_Custom "WidgetCustom"
#define __qdf_Widget_Copy "WidgetCopy"
#define __qdf_Action_Invisible "__qdf_Action_Invisible"
#define __qdf_Quick_Access_Button "__qdf_Quick_Access_Button"


#define QDF_DIC_TAGNAME_RIBBON "RibbonStorage"
#define QDF_DIC_TAGNAME_RIBBON_QABAR "QuickAccessBar"
#define QDF_DIC_TAGNAME_RIBBON_BAR "QdfRibbonBar"
#define QDF_DIC_TAGNAME_RIBBON_PAGE "QdfRibbonPage"
#define QDF_DIC_TAGNAME_RIBBON_GROUP "QdfRibbonGroup"
#define QDF_DIC_TAGNAME_RIBBON_ACTION "RibbonAction"

#define QDF_DIC_ATTRNAME_RIBBON_ID "nameId"
#define QDF_DIC_ATTRNAME_RIBBON_TITLE "title"
#define QDF_DIC_ATTRNAME_RIBBON_VISIBLE "visible"
#define QDF_DIC_ATTRNAME_RIBBON_TYPE "type"

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBON_DEF_H