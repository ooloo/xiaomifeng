#ifndef __TAG_DEF_H__
#define __TAG_DEF_H__

typedef enum _html_tag_type_t {
    TAG_A,
    TAG_FRAMESET,
    TAG_FRAME,
    TAG_NOFRAME,
    TAG_IFRAME,
    TAG_TT,
    TAG_DT,
    TAG_I,
    TAG_TD,
    TAG_DIV,
    TAG_DD,
    TAG_THEAD,
    TAG_Q,
    TAG_TH,
    TAG_TABLE,
    TAG_LI,
    TAG_TEXTAREA,
    TAG_HTML,
    TAG_TITLE,
    TAG_OBJECT,
    TAG_OL,
    TAG_BDO,
    TAG_LABEL,
    TAG_DL,
    TAG_NOSCRIPT,
    TAG_HEAD,
    TAG_INPUT,
    TAG_ADDRESS,
    TAG_DEL,
    TAG_BODY,
    TAG_TBODY,
    TAG_LEGEND,
    TAG_UL,
    TAG_BASE,
    TAG_TFOOT,
    TAG_B,
    TAG_H1,
    TAG_VAR,
    TAG_AREA,
    TAG_STYLE,
    TAG_SELECT,
    TAG_FIELDSET,
    TAG_H2,
    TAG_BIG,
    TAG_STRONG,
    TAG_INS,
    TAG_LINK,
    TAG_BLOCKQUOTE,
    TAG_H6,
    TAG_SUB,
    TAG_BUTTON,
    TAG_KBD,
    TAG_ABBR,
    TAG_H3,
    TAG_DFN,
    TAG_META,
    TAG_COL,
    TAG_H4,
    TAG_CODE,
    TAG_IMG,
    TAG_CITE,
    TAG_TR,
    TAG_SCRIPT,
    TAG_CAPTION,
    TAG_FORM,
    TAG_SMALL,
    TAG_HR,
    TAG_SAMP,
    TAG_OPTION,
    TAG_H5,
    TAG_BR,
    TAG_SUP,
    TAG_ACRONYM,
    TAG_EM,
    TAG_MAP,
    TAG_SPAN,
    TAG_PARAM,
    TAG_CENTER,
    TAG_PRE,
    TAG_COLGROUP,
    TAG_P,
    TAG_OPTGROUP,
    TAG_EMBED,
    TAG_MARQUEE,
    TAG_FONT,
    TAG_DIR,
    TAG_MENU,
    TAG_APPLET,
    TAG_BASEFONT,
    TAG_S,
    TAG_STRIKE,
    TAG_U,
    TAG_ISINDEX,
    TAG_SOUND,
    TAG_BGSOUND,
    TAG_BLINK,
    TAG_COMMENT2,
    TAG_MULTICOL,
    TAG_NOBR,
    TAG_NOEMBED,
    TAG_SPACER,
    TAG_WEB,
    TAG_LAYER,
    TAG_NOLAYER,
    TAG_WBR,

    TAG_COMMENT,
    TAG_DOCTYPE,
    TAG_PURETEXT,
    TAG_ROOT,
    TAG_UNKNOWN
} html_tag_type_t;


typedef struct _html_tag_info_t {
    const char *name;
    html_tag_type_t type;
    const char mixed, empty, etag, pre, break_before, break_after;
    const char *parents[104];
} html_tag_info_t;

/*
 * table of tag info
 */
extern html_tag_info_t html_tag_info_array[];

/*
 * size of table of tag info
 */
extern int html_tag_info_size;

#endif
