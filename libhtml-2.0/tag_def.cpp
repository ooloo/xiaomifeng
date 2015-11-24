#include <stdlib.h>
#include "tag_def.h"

html_tag_info_t html_tag_info_array[] = {
    {"a", TAG_A, 1, 0, 1, 0, 0, 0, {    // 62 parents

                                    "abbr", "acronym", "address", "applet",
                                    "b",
                                    "bdo", "big", "blink", "blockquote",
                                    "body",
                                    "caption", "center", "cite", "code",
                                    "dd",
                                    "del", "dfn", "div", "dl", "dt",
                                    "em", "fieldset", "font", "form", "h1",
                                    "h2", "h3", "h4", "h5", "h6",
                                    "html", "i", "iframe", "ins", "kbd",
                                    "label", "layer", "legend", "li",
                                    "marquee",
                                    "multicol", "nobr", "noframes",
                                    "nolayer", "noscript",
                                    "object", "p", "pre", "q", "s",
                                    "samp", "small", "span", "strike",
                                    "strong",
                                    "sub", "sup", "td", "th", "tt",
                                    "u", "var",
                                    NULL}
     }
    ,
    {"frameset", TAG_FRAMESET, 0, 0, 1, 0, 1, 1, {      // 3 parents

                                                  "body", "frameset",
                                                  "html",
                                                  NULL}
     }
    ,
    {"frame", TAG_FRAME, 0, 0, 1, 0, 1, 1, {    // 4 parents

                                            "body", "frameset", "html",
                                            "td",
                                            NULL}
     }
    ,
    {"noframes", TAG_NOFRAME, 0, 0, 1, 0, 1, 1, {       // 22 parents

                                                 "blockquote", "body",
                                                 "button", "center", "dd",
                                                 "div", "dl", "fieldset",
                                                 "form", "frameset",
                                                 "html", "iframe", "layer",
                                                 "li", "marquee",
                                                 "multicol", "noframes",
                                                 "nolayer", "noscript",
                                                 "object",
                                                 "td", "th",
                                                 NULL}
     }
    ,
    {"iframe", TAG_IFRAME, 0, 0, 1, 0, 1, 1, {  // 64 parents

                                              "a", "acronym", "address",
                                              "applet", "b",
                                              "bdo", "big", "blink",
                                              "blockquote", "body",
                                              "button", "caption",
                                              "center", "cite", "code",
                                              "dd", "del", "dfn", "div",
                                              "dl",
                                              "dt", "em", "fieldset",
                                              "font", "form",
                                              "frameset", "h1", "h2", "h3",
                                              "h4",
                                              "h5", "h6", "html", "i",
                                              "iframe",
                                              "ins", "kbd", "label",
                                              "layer", "legend",
                                              "li", "marquee", "multicol",
                                              "nobr", "noframes",
                                              "nolayer", "noscript",
                                              "object", "p", "pre",
                                              "q", "s", "samp", "small",
                                              "span",
                                              "strike", "strong", "sub",
                                              "sup", "td",
                                              "th", "tt", "u", "var",
                                              NULL}
     }
    ,
    {"tt", TAG_TT, 1, 0, 1, 0, 0, 0, {  // 64 parents

                                      "a", "abbr", "acronym", "address",
                                      "applet",
                                      "b", "bdo", "big", "blink",
                                      "blockquote",
                                      "body", "button", "caption",
                                      "center", "cite",
                                      "code", "dd", "del", "dfn", "div",
                                      "dl", "dt", "em", "fieldset", "font",
                                      "form", "h1", "h2", "h3", "h4",
                                      "h5", "h6", "html", "i", "iframe",
                                      "ins", "kbd", "label", "layer",
                                      "legend",
                                      "li", "marquee", "multicol", "nobr",
                                      "noframes",
                                      "nolayer", "noscript", "object", "p",
                                      "pre",
                                      "q", "s", "samp", "small", "span",
                                      "strike", "strong", "sub", "sup",
                                      "td",
                                      "th", "tt", "u", "var",
                                      NULL}
     }
    ,
    {"dt", TAG_DT, 1, 0, 0, 0, 1, 1, {  // 3 parents

                                      "body", "dl", "html",
                                      NULL}
     }
    ,
    {"i", TAG_I, 1, 0, 1, 0, 0, 0, {    // 64 parents

                                    "a", "abbr", "acronym", "address",
                                    "applet",
                                    "b", "bdo", "big", "blink",
                                    "blockquote",
                                    "body", "button", "caption", "center",
                                    "cite",
                                    "code", "dd", "del", "dfn", "div",
                                    "dl", "dt", "em", "fieldset", "font",
                                    "form", "h1", "h2", "h3", "h4",
                                    "h5", "h6", "html", "i", "iframe",
                                    "ins", "kbd", "label", "layer",
                                    "legend",
                                    "li", "marquee", "multicol", "nobr",
                                    "noframes",
                                    "nolayer", "noscript", "object", "p",
                                    "pre",
                                    "q", "s", "samp", "small", "span",
                                    "strike", "strong", "sub", "sup", "td",
                                    "th", "tt", "u", "var",
                                    NULL}
     }
    ,
    {"td", TAG_TD, 1, 0, 0, 0, 1, 1, {  // 4 parents

                                      "body", "html", "table", "tr",
                                      NULL}
     }
    ,
    {"div", TAG_DIV, 1, 0, 1, 0, 1, 1, {        // 25 parents

                                        "blockquote", "body", "button",
                                        "center", "dd",
                                        "del", "div", "dl", "dt",
                                        "fieldset",
                                        "form", "html", "iframe", "ins",
                                        "layer",
                                        "li", "map", "marquee", "multicol",
                                        "noframes",
                                        "nolayer", "noscript", "object",
                                        "td", "th",
                                        NULL}
     }
    ,
    {"dd", TAG_DD, 1, 0, 0, 0, 1, 1, {  // 3 parents

                                      "body", "dl", "html",
                                      NULL}
     }
    ,
    {"thead", TAG_THEAD, 0, 0, 0, 0, 1, 1, {    // 3 parents

                                            "body", "html", "table",
                                            NULL}
     }
    ,
    {"q", TAG_Q, 1, 0, 1, 0, 0, 0, {    // 64 parents

                                    "a", "abbr", "acronym", "address",
                                    "applet",
                                    "b", "bdo", "big", "blink",
                                    "blockquote",
                                    "body", "button", "caption", "center",
                                    "cite",
                                    "code", "dd", "del", "dfn", "div",
                                    "dl", "dt", "em", "fieldset", "font",
                                    "form", "h1", "h2", "h3", "h4",
                                    "h5", "h6", "html", "i", "iframe",
                                    "ins", "kbd", "label", "layer",
                                    "legend",
                                    "li", "marquee", "multicol", "nobr",
                                    "noframes",
                                    "nolayer", "noscript", "object", "p",
                                    "pre",
                                    "q", "s", "samp", "small", "span",
                                    "strike", "strong", "sub", "sup", "td",
                                    "th", "tt", "u", "var",
                                    NULL}
     }
    ,
    {"th", TAG_TH, 1, 0, 0, 0, 1, 1, {  // 3 parents

                                      "body", "html", "tr",
                                      NULL}
     }
    ,
    {"table", TAG_TABLE, 0, 0, 1, 1, 1, 1, {    // 30 parents

                                            "applet", "blockquote", "body",
                                            "button", "center",
                                            "dd", "del", "div", "dl", "dt",
                                            "embed", "fieldset", "form",
                                            "html", "iframe",
                                            "ins", "layer", "li", "map",
                                            "marquee",
                                            "multicol", "noembed",
                                            "noframes", "nolayer",
                                            "noscript",
                                            "object", "p", "pre", "td",
                                            "th",
                                            NULL}
     }
    ,
    {"li", TAG_LI, 1, 0, 0, 0, 1, 1, {  // 6 parents

                                      "body", "dir", "html", "menu", "ol",
                                      "ul",
                                      NULL}
     }
    ,
    {"textarea", TAG_TEXTAREA, 1, 0, 1, 0, 0, 0, {      // 57 parents

                                                  "a", "abbr", "acronym",
                                                  "address", "applet",
                                                  "b", "bdo", "big",
                                                  "blockquote", "body",
                                                  "caption", "center",
                                                  "cite", "code", "dd",
                                                  "del", "dfn", "div",
                                                  "dl", "dt",
                                                  "em", "fieldset", "font",
                                                  "form", "h1",
                                                  "h2", "h3", "h4", "h5",
                                                  "h6",
                                                  "html", "i", "iframe",
                                                  "ins", "kbd",
                                                  "label", "legend", "li",
                                                  "noframes", "noscript",
                                                  "object", "p", "pre",
                                                  "q", "s",
                                                  "samp", "small", "span",
                                                  "strike", "strong",
                                                  "sub", "sup", "td", "th",
                                                  "tt",
                                                  "u", "var",
                                                  NULL}
     }
    ,
    {"html", TAG_HTML, 0, 0, 1, 0, 1, 1, {      // 0 parents

                                          NULL}
     }
    ,
    {"title", TAG_TITLE, 1, 0, 1, 0, 1, 1, {    // 2 parents

                                            "head", "html",
                                            NULL}
     }
    ,
    {"object", TAG_OBJECT, 1, 0, 1, 0, 0, 0, {  // 65 parents

                                              "a", "abbr", "acronym",
                                              "address", "applet",
                                              "b", "bdo", "big", "blink",
                                              "blockquote",
                                              "body", "button", "caption",
                                              "center", "cite",
                                              "code", "dd", "del", "dfn",
                                              "div",
                                              "dl", "dt", "em", "fieldset",
                                              "font",
                                              "form", "h1", "h2", "h3",
                                              "h4",
                                              "h5", "h6", "head", "html",
                                              "i",
                                              "iframe", "ins", "kbd",
                                              "label", "layer",
                                              "legend", "li", "marquee",
                                              "multicol", "nobr",
                                              "noframes", "nolayer",
                                              "noscript", "object", "p",
                                              "pre", "q", "s", "samp",
                                              "small",
                                              "span", "strike", "strong",
                                              "sub", "sup",
                                              "td", "th", "tt", "u", "var",
                                              NULL}
     }
    ,
    {"ol", TAG_OL, 0, 0, 1, 0, 1, 1, {  // 24 parents

                                      "blockquote", "body", "button",
                                      "center", "dd",
                                      "del", "div", "dl", "fieldset",
                                      "form",
                                      "html", "iframe", "ins", "layer",
                                      "li",
                                      "map", "marquee", "multicol",
                                      "noframes", "nolayer",
                                      "noscript", "object", "td", "th",
                                      NULL}
     }
    ,
    {"bdo", TAG_BDO, 1, 0, 1, 0, 0, 0, {        // 53 parents

                                        "a", "abbr", "acronym", "address",
                                        "b",
                                        "bdo", "big", "blink", "body",
                                        "button",
                                        "caption", "cite", "code", "dd",
                                        "del",
                                        "dfn", "div", "dl", "dt", "em",
                                        "fieldset", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "ins",
                                        "kbd", "label", "layer", "legend",
                                        "li",
                                        "marquee", "multicol", "nobr",
                                        "nolayer", "object",
                                        "p", "pre", "q", "samp", "small",
                                        "span", "strong", "sub", "sup",
                                        "td",
                                        "th", "tt", "var",
                                        NULL}
     }
    ,
    {"label", TAG_LABEL, 1, 0, 1, 0, 0, 0, {    // 56 parents

                                            "a", "abbr", "acronym",
                                            "address", "applet",
                                            "b", "bdo", "big",
                                            "blockquote", "body",
                                            "caption", "center", "cite",
                                            "code", "dd",
                                            "del", "dfn", "div", "dl",
                                            "dt",
                                            "em", "fieldset", "font",
                                            "form", "h1",
                                            "h2", "h3", "h4", "h5", "h6",
                                            "html", "i", "iframe", "ins",
                                            "kbd",
                                            "legend", "li", "noframes",
                                            "noscript", "object",
                                            "p", "pre", "q", "s", "samp",
                                            "small", "span", "strike",
                                            "strong", "sub",
                                            "sup", "td", "th", "tt", "u",
                                            "var",
                                            NULL}
     }
    ,
    {"dl", TAG_DL, 0, 0, 1, 0, 1, 1, {  // 24 parents

                                      "blockquote", "body", "button",
                                      "center", "dd",
                                      "del", "div", "dl", "fieldset",
                                      "form",
                                      "html", "iframe", "ins", "layer",
                                      "li",
                                      "map", "marquee", "multicol",
                                      "noframes", "nolayer",
                                      "noscript", "object", "td", "th",
                                      NULL}
     }
    ,
    {"noscript", TAG_NOSCRIPT, 0, 0, 1, 0, 1, 1, {      // 24 parents

                                                  "blockquote", "body",
                                                  "button", "center", "dd",
                                                  "del", "div", "dl",
                                                  "fieldset", "form",
                                                  "html", "iframe", "ins",
                                                  "layer", "li",
                                                  "map", "marquee",
                                                  "multicol", "noframes",
                                                  "nolayer",
                                                  "noscript", "object",
                                                  "td", "th",
                                                  NULL}
     }
    ,
    {"head", TAG_HEAD, 0, 0, 1, 0, 1, 1, {      // 1 parents

                                          "html",
                                          NULL}
     }
    ,
    {"input", TAG_INPUT, 0, 1, 0, 0, 0, 0, {    // 57 parents

                                            "a", "abbr", "acronym",
                                            "address", "applet",
                                            "b", "bdo", "big",
                                            "blockquote", "body",
                                            "caption", "center", "cite",
                                            "code", "dd",
                                            "del", "dfn", "div", "dl",
                                            "dt",
                                            "em", "fieldset", "font",
                                            "form", "h1",
                                            "h2", "h3", "h4", "h5", "h6",
                                            "html", "i", "iframe", "ins",
                                            "kbd",
                                            "label", "legend", "li",
                                            "noframes", "noscript",
                                            "object", "p", "pre", "q", "s",
                                            "samp", "small", "span",
                                            "strike", "strong",
                                            "sub", "sup", "td", "th", "tt",
                                            "u", "var",
                                            NULL}
     }
    ,
    {"address", TAG_ADDRESS, 1, 0, 1, 0, 1, 1, {        // 24 parents

                                                "blockquote", "body",
                                                "button", "center", "dd",
                                                "del", "div", "dl",
                                                "fieldset", "form",
                                                "html", "iframe", "ins",
                                                "layer", "li",
                                                "map", "marquee",
                                                "multicol", "noframes",
                                                "nolayer",
                                                "noscript", "object", "td",
                                                "th",
                                                NULL}
     }
    ,
    {"del", TAG_DEL, 1, 0, 1, 0, 0, 0, {        // 3 parents

                                        "body", "center", "html",
                                        NULL}
     }
    ,
    {"body", TAG_BODY, 0, 0, 1, 0, 1, 1, {      // 2 parents

                                          "html", "noframes",
                                          NULL}
     }
    ,
    {"tbody", TAG_TBODY, 0, 0, 0, 0, 1, 1, {    // 3 parents

                                            "body", "html", "table",
                                            NULL}
     }
    ,
    {"legend", TAG_LEGEND, 1, 0, 1, 0, 1, 1, {  // 3 parents

                                              "body", "fieldset", "html",
                                              NULL}
     }
    ,
    {"ul", TAG_UL, 0, 0, 1, 0, 1, 1, {  // 24 parents

                                      "blockquote", "body", "button",
                                      "center", "dd",
                                      "del", "div", "dl", "fieldset",
                                      "form",
                                      "html", "iframe", "ins", "layer",
                                      "li",
                                      "map", "marquee", "multicol",
                                      "noframes", "nolayer",
                                      "noscript", "object", "td", "th",
                                      NULL}
     }
    ,
    {"base", TAG_BASE, 0, 1, 0, 0, 1, 1, {      // 3 parents

                                          "body", "head", "html",
                                          NULL}
     }
    ,
    {"tfoot", TAG_TFOOT, 0, 0, 0, 0, 1, 1, {    // 3 parents

                                            "body", "html", "table",
                                            NULL}
     }
    ,
    {"b", TAG_B, 1, 0, 1, 0, 0, 0, {    // 64 parents

                                    "a", "abbr", "acronym", "address",
                                    "applet",
                                    "b", "bdo", "big", "blink",
                                    "blockquote",
                                    "body", "button", "caption", "center",
                                    "cite",
                                    "code", "dd", "del", "dfn", "div",
                                    "dl", "dt", "em", "fieldset", "font",
                                    "form", "h1", "h2", "h3", "h4",
                                    "h5", "h6", "html", "i", "iframe",
                                    "ins", "kbd", "label", "layer",
                                    "legend",
                                    "li", "marquee", "multicol", "nobr",
                                    "noframes",
                                    "nolayer", "noscript", "object", "p",
                                    "pre",
                                    "q", "s", "samp", "small", "span",
                                    "strike", "strong", "sub", "sup", "td",
                                    "th", "tt", "u", "var",
                                    NULL}
     }
    ,
    {"h1", TAG_H1, 1, 0, 1, 0, 1, 1, {  // 25 parents

                                      "a", "blockquote", "body", "button",
                                      "center",
                                      "dd", "del", "div", "dl", "fieldset",
                                      "form", "html", "iframe", "ins",
                                      "layer",
                                      "li", "map", "marquee", "multicol",
                                      "noframes",
                                      "nolayer", "noscript", "object",
                                      "td", "th",
                                      NULL}
     }
    ,
    {"var", TAG_VAR, 1, 0, 1, 0, 0, 0, {        // 64 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "pre",
                                        "q", "s", "samp", "small", "span",
                                        "strike", "strong", "sub", "sup",
                                        "td",
                                        "th", "tt", "u", "var",
                                        NULL}
     }
    ,
    {"area", TAG_AREA, 0, 1, 0, 0, 0, 0, {      // 3 parents

                                          "body", "html", "map",
                                          NULL}
     }
    ,
    {"style", TAG_STYLE, 1, 0, 1, 1, 1, 1, {    // 4 parents

                                            "body", "center", "head",
                                            "html",
                                            NULL}
     }
    ,
    {"select", TAG_SELECT, 0, 0, 1, 0, 0, 0, {  // 57 parents

                                              "a", "abbr", "acronym",
                                              "address", "applet",
                                              "b", "bdo", "big",
                                              "blockquote", "body",
                                              "caption", "center", "cite",
                                              "code", "dd",
                                              "del", "dfn", "div", "dl",
                                              "dt",
                                              "em", "fieldset", "font",
                                              "form", "h1",
                                              "h2", "h3", "h4", "h5", "h6",
                                              "html", "i", "iframe", "ins",
                                              "kbd",
                                              "label", "legend", "li",
                                              "noframes", "noscript",
                                              "object", "p", "pre", "q",
                                              "s",
                                              "samp", "small", "span",
                                              "strike", "strong",
                                              "sub", "sup", "td", "th",
                                              "tt",
                                              "u", "var",
                                              NULL}
     }
    ,
    {"fieldset", TAG_FIELDSET, 1, 0, 1, 0, 1, 1, {      // 22 parents

                                                  "blockquote", "body",
                                                  "center", "dd", "del",
                                                  "div", "fieldset",
                                                  "form", "html", "iframe",
                                                  "ins", "layer", "li",
                                                  "map", "marquee",
                                                  "multicol", "noframes",
                                                  "nolayer", "noscript",
                                                  "object",
                                                  "td", "th",
                                                  NULL}
     }
    ,
    {"h2", TAG_H2, 1, 0, 1, 0, 1, 1, {  // 23 parents

                                      "a", "blockquote", "body", "button",
                                      "center",
                                      "dd", "del", "div", "dl", "fieldset",
                                      "form", "html", "ins", "layer", "li",
                                      "map", "marquee", "multicol",
                                      "nolayer", "noscript",
                                      "object", "td", "th",
                                      NULL}
     }
    ,
    {"big", TAG_BIG, 1, 0, 1, 0, 0, 0, {        // 63 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "q",
                                        "s", "samp", "small", "span",
                                        "strike",
                                        "strong", "sub", "sup", "td", "th",
                                        "tt", "u", "var",
                                        NULL}
     }
    ,
    {"strong", TAG_STRONG, 1, 0, 1, 0, 0, 0, {  // 64 parents

                                              "a", "abbr", "acronym",
                                              "address", "applet",
                                              "b", "bdo", "big", "blink",
                                              "blockquote",
                                              "body", "button", "caption",
                                              "center", "cite",
                                              "code", "dd", "del", "dfn",
                                              "div",
                                              "dl", "dt", "em", "fieldset",
                                              "font",
                                              "form", "h1", "h2", "h3",
                                              "h4",
                                              "h5", "h6", "html", "i",
                                              "iframe",
                                              "ins", "kbd", "label",
                                              "layer", "legend",
                                              "li", "marquee", "multicol",
                                              "nobr", "noframes",
                                              "nolayer", "noscript",
                                              "object", "p", "pre",
                                              "q", "s", "samp", "small",
                                              "span",
                                              "strike", "strong", "sub",
                                              "sup", "td",
                                              "th", "tt", "u", "var",
                                              NULL}
     }
    ,
    {"ins", TAG_INS, 1, 0, 1, 0, 0, 0, {        // 3 parents

                                        "body", "center", "html",
                                        NULL}
     }
    ,
    {"link", TAG_LINK, 0, 1, 0, 0, 1, 1, {      // 2 parents

                                          "head", "html",
                                          NULL}
     }
    ,
    {"blockquote", TAG_BLOCKQUOTE, 0, 0, 1, 0, 1, 1, {  // 24 parents

                                                      "blockquote", "body",
                                                      "button", "center",
                                                      "dd",
                                                      "del", "div", "dl",
                                                      "fieldset", "form",
                                                      "html", "iframe",
                                                      "ins", "layer", "li",
                                                      "map", "marquee",
                                                      "multicol",
                                                      "noframes",
                                                      "nolayer",
                                                      "noscript", "object",
                                                      "td", "th",
                                                      NULL}
     }
    ,
    {"h6", TAG_H6, 1, 0, 1, 0, 1, 1, {  // 23 parents

                                      "a", "blockquote", "body", "button",
                                      "center",
                                      "dd", "del", "div", "dl", "fieldset",
                                      "form", "html", "ins", "layer", "li",
                                      "map", "marquee", "multicol",
                                      "nolayer", "noscript",
                                      "object", "td", "th",
                                      NULL}
     }
    ,
    {"sub", TAG_SUB, 1, 0, 1, 0, 0, 0, {        // 63 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "q",
                                        "s", "samp", "small", "span",
                                        "strike",
                                        "strong", "sub", "sup", "td", "th",
                                        "tt", "u", "var",
                                        NULL}
     }
    ,
    {"button", TAG_BUTTON, 1, 0, 1, 0, 1, 1, {  // 57 parents

                                              "a", "abbr", "acronym",
                                              "address", "applet",
                                              "b", "bdo", "big",
                                              "blockquote", "body",
                                              "caption", "center", "cite",
                                              "code", "dd",
                                              "del", "dfn", "div", "dl",
                                              "dt",
                                              "em", "fieldset", "font",
                                              "form", "h1",
                                              "h2", "h3", "h4", "h5", "h6",
                                              "html", "i", "iframe", "ins",
                                              "kbd",
                                              "label", "legend", "li",
                                              "noframes", "noscript",
                                              "object", "p", "pre", "q",
                                              "s",
                                              "samp", "small", "span",
                                              "strike", "strong",
                                              "sub", "sup", "td", "th",
                                              "tt",
                                              "u", "var",
                                              NULL}
     }
    ,
    {"kbd", TAG_KBD, 1, 0, 1, 0, 0, 0, {        // 64 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "pre",
                                        "q", "s", "samp", "small", "span",
                                        "strike", "strong", "sub", "sup",
                                        "td",
                                        "th", "tt", "u", "var",
                                        NULL}
     }
    ,
    {"abbr", TAG_ABBR, 1, 0, 1, 0, 0, 0, {      // 53 parents

                                          "a", "abbr", "acronym",
                                          "address", "b",
                                          "bdo", "big", "blink", "body",
                                          "button",
                                          "caption", "cite", "code", "dd",
                                          "del",
                                          "dfn", "div", "dl", "dt", "em",
                                          "fieldset", "h1", "h2", "h3",
                                          "h4",
                                          "h5", "h6", "html", "i", "ins",
                                          "kbd", "label", "layer",
                                          "legend", "li",
                                          "marquee", "multicol", "nobr",
                                          "nolayer", "object",
                                          "p", "pre", "q", "samp", "small",
                                          "span", "strong", "sub", "sup",
                                          "td",
                                          "th", "tt", "var",
                                          NULL}
     }
    ,
    {"h3", TAG_H3, 1, 0, 1, 0, 1, 1, {  // 23 parents

                                      "a", "blockquote", "body", "button",
                                      "center",
                                      "dd", "del", "div", "dl", "fieldset",
                                      "form", "html", "ins", "layer", "li",
                                      "map", "marquee", "multicol",
                                      "nolayer", "noscript",
                                      "object", "td", "th",
                                      NULL}
     }
    ,
    {"dfn", TAG_DFN, 1, 0, 1, 0, 0, 0, {        // 64 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "pre",
                                        "q", "s", "samp", "small", "span",
                                        "strike", "strong", "sub", "sup",
                                        "td",
                                        "th", "tt", "u", "var",
                                        NULL}
     }
    ,
    {"meta", TAG_META, 0, 1, 0, 0, 1, 1, {      // 3 parents

                                          "body", "head", "html",
                                          NULL}
     }
    ,
    {"col", TAG_COL, 0, 1, 0, 0, 0, 0, {        // 4 parents

                                        "body", "colgroup", "html",
                                        "table",
                                        NULL}
     }
    ,
    {"h4", TAG_H4, 1, 0, 1, 0, 1, 1, {  // 23 parents

                                      "a", "blockquote", "body", "button",
                                      "center",
                                      "dd", "del", "div", "dl", "fieldset",
                                      "form", "html", "ins", "layer", "li",
                                      "map", "marquee", "multicol",
                                      "nolayer", "noscript",
                                      "object", "td", "th",
                                      NULL}
     }
    ,
    {"code", TAG_CODE, 1, 0, 1, 0, 0, 0, {      // 64 parents

                                          "a", "abbr", "acronym",
                                          "address", "applet",
                                          "b", "bdo", "big", "blink",
                                          "blockquote",
                                          "body", "button", "caption",
                                          "center", "cite",
                                          "code", "dd", "del", "dfn",
                                          "div",
                                          "dl", "dt", "em", "fieldset",
                                          "font",
                                          "form", "h1", "h2", "h3", "h4",
                                          "h5", "h6", "html", "i",
                                          "iframe",
                                          "ins", "kbd", "label", "layer",
                                          "legend",
                                          "li", "marquee", "multicol",
                                          "nobr", "noframes",
                                          "nolayer", "noscript", "object",
                                          "p", "pre",
                                          "q", "s", "samp", "small",
                                          "span",
                                          "strike", "strong", "sub", "sup",
                                          "td",
                                          "th", "tt", "u", "var",
                                          NULL}
     }
    ,
    {"img", TAG_IMG, 0, 1, 0, 0, 0, 0, {        // 63 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "q",
                                        "s", "samp", "small", "span",
                                        "strike",
                                        "strong", "sub", "sup", "td", "th",
                                        "tt", "u", "var",
                                        NULL}
     }
    ,
    {"cite", TAG_CITE, 1, 0, 1, 0, 0, 0, {      // 64 parents

                                          "a", "abbr", "acronym",
                                          "address", "applet",
                                          "b", "bdo", "big", "blink",
                                          "blockquote",
                                          "body", "button", "caption",
                                          "center", "cite",
                                          "code", "dd", "del", "dfn",
                                          "div",
                                          "dl", "dt", "em", "fieldset",
                                          "font",
                                          "form", "h1", "h2", "h3", "h4",
                                          "h5", "h6", "html", "i",
                                          "iframe",
                                          "ins", "kbd", "label", "layer",
                                          "legend",
                                          "li", "marquee", "multicol",
                                          "nobr", "noframes",
                                          "nolayer", "noscript", "object",
                                          "p", "pre",
                                          "q", "s", "samp", "small",
                                          "span",
                                          "strike", "strong", "sub", "sup",
                                          "td",
                                          "th", "tt", "u", "var",
                                          NULL}
     }
    ,
    {"tr", TAG_TR, 0, 0, 0, 0, 1, 1, {  // 6 parents

                                      "body", "html", "table", "tbody",
                                      "tfoot",
                                      "thead",
                                      NULL}
     }
    ,
    {"script", TAG_SCRIPT, 1, 0, 1, 1, 0, 0, {  // 65 parents

                                              "a", "abbr", "acronym",
                                              "address", "applet",
                                              "b", "bdo", "big", "blink",
                                              "blockquote",
                                              "body", "button", "caption",
                                              "center", "cite",
                                              "code", "dd", "del", "dfn",
                                              "div",
                                              "dl", "dt", "em", "fieldset",
                                              "font",
                                              "form", "h1", "h2", "h3",
                                              "h4",
                                              "h5", "h6", "head", "html",
                                              "i",
                                              "iframe", "ins", "kbd",
                                              "label", "layer",
                                              "legend", "li", "marquee",
                                              "multicol", "nobr",
                                              "noframes", "nolayer",
                                              "noscript", "object", "p",
                                              "pre", "q", "s", "samp",
                                              "small",
                                              "span", "strike", "strong",
                                              "sub", "sup",
                                              "td", "th", "tt", "u", "var",
                                              NULL}
     }
    ,
    {"caption", TAG_CAPTION, 1, 0, 1, 0, 1, 1, {        // 3 parents

                                                "body", "html", "table",
                                                NULL}
     }
    ,
    {"form", TAG_FORM, 0, 0, 1, 0, 1, 1, {      // 24 parents

                                          "blockquote", "body", "center",
                                          "dd", "del",
                                          "div", "dl", "fieldset", "html",
                                          "iframe",
                                          "ins", "layer", "li", "map",
                                          "marquee",
                                          "multicol", "noframes",
                                          "nolayer", "noscript", "object",
                                          "table", "td", "th", "tr",
                                          NULL}
     }
    ,
    {"small", TAG_SMALL, 1, 0, 1, 0, 0, 0, {    // 63 parents

                                            "a", "abbr", "acronym",
                                            "address", "applet",
                                            "b", "bdo", "big", "blink",
                                            "blockquote",
                                            "body", "button", "caption",
                                            "center", "cite",
                                            "code", "dd", "del", "dfn",
                                            "div",
                                            "dl", "dt", "em", "fieldset",
                                            "font",
                                            "form", "h1", "h2", "h3", "h4",
                                            "h5", "h6", "html", "i",
                                            "iframe",
                                            "ins", "kbd", "label", "layer",
                                            "legend",
                                            "li", "marquee", "multicol",
                                            "nobr", "noframes",
                                            "nolayer", "noscript",
                                            "object", "p", "q",
                                            "s", "samp", "small", "span",
                                            "strike",
                                            "strong", "sub", "sup", "td",
                                            "th",
                                            "tt", "u", "var",
                                            NULL}
     }
    ,
    {"hr", TAG_HR, 0, 1, 0, 0, 1, 1, {  // 24 parents

                                      "blockquote", "body", "button",
                                      "center", "dd",
                                      "del", "div", "dl", "fieldset",
                                      "form",
                                      "html", "iframe", "ins", "layer",
                                      "li",
                                      "map", "marquee", "multicol",
                                      "noframes", "nolayer",
                                      "noscript", "object", "td", "th",
                                      NULL}
     }
    ,
    {"samp", TAG_SAMP, 1, 0, 1, 0, 0, 0, {      // 64 parents

                                          "a", "abbr", "acronym",
                                          "address", "applet",
                                          "b", "bdo", "big", "blink",
                                          "blockquote",
                                          "body", "button", "caption",
                                          "center", "cite",
                                          "code", "dd", "del", "dfn",
                                          "div",
                                          "dl", "dt", "em", "fieldset",
                                          "font",
                                          "form", "h1", "h2", "h3", "h4",
                                          "h5", "h6", "html", "i",
                                          "iframe",
                                          "ins", "kbd", "label", "layer",
                                          "legend",
                                          "li", "marquee", "multicol",
                                          "nobr", "noframes",
                                          "nolayer", "noscript", "object",
                                          "p", "pre",
                                          "q", "s", "samp", "small",
                                          "span",
                                          "strike", "strong", "sub", "sup",
                                          "td",
                                          "th", "tt", "u", "var",
                                          NULL}
     }
    ,
    {"option", TAG_OPTION, 1, 0, 0, 0, 1, 1, {  // 4 parents

                                              "body", "html", "optgroup",
                                              "select",
                                              NULL}
     }
    ,
    {"h5", TAG_H5, 1, 0, 1, 0, 1, 1, {  // 23 parents

                                      "a", "blockquote", "body", "button",
                                      "center",
                                      "dd", "del", "div", "dl", "fieldset",
                                      "form", "html", "ins", "layer", "li",
                                      "map", "marquee", "multicol",
                                      "nolayer", "noscript",
                                      "object", "td", "th",
                                      NULL}
     }
    ,
    {"br", TAG_BR, 0, 1, 0, 0, 0, 1, {  // 63 parents

                                      "a", "abbr", "acronym", "address",
                                      "applet",
                                      "b", "bdo", "big", "blockquote",
                                      "body",
                                      "button", "caption", "center",
                                      "cite", "code",
                                      "dd", "del", "dfn", "div", "dl",
                                      "dt", "em", "fieldset", "font",
                                      "form",
                                      "h1", "h2", "h3", "h4", "h5",
                                      "h6", "html", "i", "iframe", "ins",
                                      "kbd", "label", "layer", "legend",
                                      "li",
                                      "marquee", "multicol", "noframes",
                                      "nolayer", "noscript",
                                      "object", "p", "pre", "q", "s",
                                      "samp", "small", "span", "strike",
                                      "strong",
                                      "sub", "sup", "td", "th", "tr",
                                      "tt", "u", "var",
                                      NULL}
     }
    ,
    {"sup", TAG_SUP, 1, 0, 1, 0, 0, 0, {        // 63 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "q",
                                        "s", "samp", "small", "span",
                                        "strike",
                                        "strong", "sub", "sup", "td", "th",
                                        "tt", "u", "var",
                                        NULL}
     }
    ,
    {"acronym", TAG_ACRONYM, 1, 0, 1, 0, 0, 0, {        // 58 parents

                                                "a", "abbr", "acronym",
                                                "address", "applet",
                                                "b", "bdo", "big",
                                                "blockquote", "body",
                                                "button", "caption",
                                                "center", "cite", "code",
                                                "dd", "del", "dfn", "div",
                                                "dl",
                                                "dt", "em", "fieldset",
                                                "font", "form",
                                                "h1", "h2", "h3", "h4",
                                                "h5",
                                                "h6", "html", "i",
                                                "iframe", "ins",
                                                "kbd", "label", "legend",
                                                "li", "noframes",
                                                "noscript", "object", "p",
                                                "pre", "q",
                                                "s", "samp", "small",
                                                "span", "strike",
                                                "strong", "sub", "sup",
                                                "td", "th",
                                                "tt", "u", "var",
                                                NULL}
     }
    ,
    {"em", TAG_EM, 1, 0, 1, 0, 0, 0, {  // 64 parents

                                      "a", "abbr", "acronym", "address",
                                      "applet",
                                      "b", "bdo", "big", "blink",
                                      "blockquote",
                                      "body", "button", "caption",
                                      "center", "cite",
                                      "code", "dd", "del", "dfn", "div",
                                      "dl", "dt", "em", "fieldset", "font",
                                      "form", "h1", "h2", "h3", "h4",
                                      "h5", "h6", "html", "i", "iframe",
                                      "ins", "kbd", "label", "layer",
                                      "legend",
                                      "li", "marquee", "multicol", "nobr",
                                      "noframes",
                                      "nolayer", "noscript", "object", "p",
                                      "pre",
                                      "q", "s", "samp", "small", "span",
                                      "strike", "strong", "sub", "sup",
                                      "td",
                                      "th", "tt", "u", "var",
                                      NULL}
     }
    ,
    {"map", TAG_MAP, 0, 0, 1, 0, 0, 0, {        // 64 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "fieldset",
                                        "font",
                                        "form", "h1", "h2", "h3", "h4",
                                        "h5", "h6", "html", "i", "iframe",
                                        "ins", "kbd", "label", "layer",
                                        "legend",
                                        "li", "marquee", "multicol",
                                        "nobr", "noframes",
                                        "nolayer", "noscript", "object",
                                        "p", "pre",
                                        "q", "s", "samp", "small", "span",
                                        "strike", "strong", "sub", "sup",
                                        "td",
                                        "th", "tt", "u", "var",
                                        NULL}
     }
    ,
    {"span", TAG_SPAN, 1, 0, 1, 0, 0, 0, {      // 64 parents

                                          "a", "abbr", "acronym",
                                          "address", "applet",
                                          "b", "bdo", "big", "blink",
                                          "blockquote",
                                          "body", "button", "caption",
                                          "center", "cite",
                                          "code", "dd", "del", "dfn",
                                          "div",
                                          "dl", "dt", "em", "fieldset",
                                          "font",
                                          "form", "h1", "h2", "h3", "h4",
                                          "h5", "h6", "html", "i",
                                          "iframe",
                                          "ins", "kbd", "label", "layer",
                                          "legend",
                                          "li", "marquee", "multicol",
                                          "nobr", "noframes",
                                          "nolayer", "noscript", "object",
                                          "p", "pre",
                                          "q", "s", "samp", "small",
                                          "span",
                                          "strike", "strong", "sub", "sup",
                                          "td",
                                          "th", "tt", "u", "var",
                                          NULL}
     }
    ,
    {"param", TAG_PARAM, 0, 1, 0, 0, 1, 1, {    // 4 parents

                                            "applet", "body", "html",
                                            "object",
                                            NULL}
     }
    ,
    {"center", TAG_CENTER, 1, 0, 1, 0, 1, 1, {  // 20 parents

                                              "blockquote", "body",
                                              "button", "center", "dd",
                                              "del", "div", "fieldset",
                                              "font", "form",
                                              "html", "iframe", "ins",
                                              "li", "map",
                                              "noframes", "noscript",
                                              "object", "td", "th",
                                              NULL}
     }
    ,
    {"pre", TAG_PRE, 1, 0, 1, 1, 1, 1, {        // 24 parents

                                        "blockquote", "body", "button",
                                        "center", "dd",
                                        "del", "div", "dl", "fieldset",
                                        "form",
                                        "html", "iframe", "ins", "layer",
                                        "li",
                                        "map", "marquee", "multicol",
                                        "noframes", "nolayer",
                                        "noscript", "object", "td", "th",
                                        NULL}
     }
    ,
    {"colgroup", TAG_COLGROUP, 0, 0, 1, 0, 1, 1, {      // 3 parents

                                                  "body", "html", "table",
                                                  NULL}
     }
    ,
    {"p", TAG_P, 1, 0, 0, 0, 1, 1, {    // 24 parents

                                    "address", "blockquote", "body",
                                    "button", "center",
                                    "dd", "del", "div", "fieldset", "form",
                                    "html", "iframe", "ins", "layer", "li",
                                    "map", "marquee", "multicol",
                                    "noframes", "nolayer",
                                    "noscript", "object", "td", "th",
                                    NULL}
     }
    ,
    {"optgroup", TAG_OPTGROUP, 0, 0, 1, 0, 1, 1, {      // 3 parents

                                                  "body", "html", "select",
                                                  NULL}
     }
    ,
    {"embed", TAG_EMBED, 0, 0, 1, 0, 1, 1, {    // 12 parents

                                            "blink", "body", "html",
                                            "layer", "marquee",
                                            "multicol", "nobr", "nolayer",
                                            "object", "table",
                                            "td", "tr",
                                            NULL}
     }
    ,
    {"marquee", TAG_MARQUEE, 0, 0, 1, 0, 1, 1, {        // 25 parents

                                                "applet", "blockquote",
                                                "body", "center", "dd",
                                                "div", "dl", "embed",
                                                "fieldset", "form",
                                                "html", "iframe", "layer",
                                                "li", "multicol",
                                                "noembed", "noframes",
                                                "nolayer", "noscript",
                                                "object",
                                                "pre", "table", "td", "th",
                                                "tr",
                                                NULL}
     }
    ,
    {"font", TAG_FONT, 0, 0, 1, 0, 0, 0, {      // 63 parents

                                          "a", "acronym", "address",
                                          "applet", "b",
                                          "bdo", "big", "blink",
                                          "blockquote", "body",
                                          "button", "caption", "center",
                                          "cite", "code",
                                          "dd", "del", "dfn", "div", "dl",
                                          "dt", "em", "fieldset", "font",
                                          "form",
                                          "h1", "h2", "h3", "h4", "h5",
                                          "h6", "html", "i", "iframe",
                                          "ins",
                                          "kbd", "label", "layer",
                                          "legend", "li",
                                          "marquee", "multicol", "nobr",
                                          "noframes", "nolayer",
                                          "noscript", "object", "p", "q",
                                          "s",
                                          "samp", "small", "span",
                                          "strike", "strong",
                                          "sub", "sup", "td", "th",
                                          "title",
                                          "tt", "u", "var",
                                          NULL}
     }
    ,
    {"dir", TAG_DIR, 0, 0, 1, 0, 1, 1, {        // 21 parents

                                        "blockquote", "body", "button",
                                        "center", "dd",
                                        "div", "dl", "fieldset", "form",
                                        "html",
                                        "iframe", "layer", "li", "marquee",
                                        "multicol",
                                        "noframes", "nolayer", "noscript",
                                        "object", "td",
                                        "th",
                                        NULL}
     }
    ,
    {"menu", TAG_MENU, 0, 0, 1, 0, 1, 1, {      // 21 parents

                                          "blockquote", "body", "button",
                                          "center", "dd",
                                          "div", "dl", "fieldset", "form",
                                          "html",
                                          "iframe", "layer", "li",
                                          "marquee", "multicol",
                                          "noframes", "nolayer",
                                          "noscript", "object", "td",
                                          "th",
                                          NULL}
     }
    ,
    {"applet", TAG_APPLET, 0, 0, 1, 0, 1, 1, {  // 63 parents

                                              "a", "acronym", "address",
                                              "applet", "b",
                                              "bdo", "big", "blink",
                                              "blockquote", "body",
                                              "button", "caption",
                                              "center", "cite", "code",
                                              "dd", "del", "dfn", "div",
                                              "dl",
                                              "dt", "em", "fieldset",
                                              "font", "form",
                                              "h1", "h2", "h3", "h4", "h5",
                                              "h6", "html", "i", "iframe",
                                              "ins",
                                              "kbd", "label", "layer",
                                              "legend", "li",
                                              "marquee", "multicol",
                                              "nobr", "noframes",
                                              "nolayer",
                                              "noscript", "object", "p",
                                              "pre", "q",
                                              "s", "samp", "small", "span",
                                              "strike",
                                              "strong", "sub", "sup", "td",
                                              "th",
                                              "tt", "u", "var",
                                              NULL}
     }
    ,
    {"basefont", TAG_BASEFONT, 0, 1, 0, 0, 0, 0, {      // 61 parents

                                                  "a", "acronym",
                                                  "address", "applet", "b",
                                                  "bdo", "big",
                                                  "blockquote", "body",
                                                  "button",
                                                  "caption", "center",
                                                  "cite", "code", "dd",
                                                  "del", "dfn", "div",
                                                  "dl", "dt",
                                                  "em", "fieldset", "font",
                                                  "form", "h1",
                                                  "h2", "h3", "h4", "h5",
                                                  "h6",
                                                  "html", "i", "iframe",
                                                  "ins", "kbd",
                                                  "label", "layer",
                                                  "legend", "li",
                                                  "marquee",
                                                  "multicol", "noframes",
                                                  "nolayer", "noscript",
                                                  "object",
                                                  "p", "pre", "q", "s",
                                                  "samp",
                                                  "small", "span",
                                                  "strike", "strong",
                                                  "sub",
                                                  "sup", "td", "th", "tt",
                                                  "u",
                                                  "var",
                                                  NULL}
     }
    ,
    {"s", TAG_S, 1, 0, 1, 0, 0, 0, {    // 63 parents

                                    "a", "acronym", "address", "applet",
                                    "b",
                                    "bdo", "big", "blink", "blockquote",
                                    "body",
                                    "button", "caption", "center", "cite",
                                    "code",
                                    "dd", "del", "dfn", "div", "dl",
                                    "dt", "em", "fieldset", "font", "form",
                                    "h1", "h2", "h3", "h4", "h5",
                                    "h6", "html", "i", "iframe", "ins",
                                    "kbd", "label", "layer", "legend",
                                    "li",
                                    "marquee", "multicol", "nobr",
                                    "noframes", "nolayer",
                                    "noscript", "object", "p", "pre", "q",
                                    "s", "samp", "small", "span", "strike",
                                    "strong", "sub", "sup", "td", "th",
                                    "tt", "u", "var",
                                    NULL}
     }
    ,
    {"strike", TAG_STRIKE, 1, 0, 1, 0, 0, 0, {  // 63 parents

                                              "a", "acronym", "address",
                                              "applet", "b",
                                              "bdo", "big", "blink",
                                              "blockquote", "body",
                                              "button", "caption",
                                              "center", "cite", "code",
                                              "dd", "del", "dfn", "div",
                                              "dl",
                                              "dt", "em", "fieldset",
                                              "font", "form",
                                              "h1", "h2", "h3", "h4", "h5",
                                              "h6", "html", "i", "iframe",
                                              "ins",
                                              "kbd", "label", "layer",
                                              "legend", "li",
                                              "marquee", "multicol",
                                              "nobr", "noframes",
                                              "nolayer",
                                              "noscript", "object", "p",
                                              "pre", "q",
                                              "s", "samp", "small", "span",
                                              "strike",
                                              "strong", "sub", "sup", "td",
                                              "th",
                                              "tt", "u", "var",
                                              NULL}
     }
    ,
    {"u", TAG_U, 1, 0, 1, 0, 0, 0, {    // 63 parents

                                    "a", "acronym", "address", "applet",
                                    "b",
                                    "bdo", "big", "blink", "blockquote",
                                    "body",
                                    "button", "caption", "center", "cite",
                                    "code",
                                    "dd", "del", "dfn", "div", "dl",
                                    "dt", "em", "fieldset", "font", "form",
                                    "h1", "h2", "h3", "h4", "h5",
                                    "h6", "html", "i", "iframe", "ins",
                                    "kbd", "label", "layer", "legend",
                                    "li",
                                    "marquee", "multicol", "nobr",
                                    "noframes", "nolayer",
                                    "noscript", "object", "p", "pre", "q",
                                    "s", "samp", "small", "span", "strike",
                                    "strong", "sub", "sup", "td", "th",
                                    "tt", "u", "var",
                                    NULL}
     }
    ,
    {"isindex", TAG_ISINDEX, 1, 1, 0, 0, 1, 1, {        // 21 parents

                                                "blockquote", "body",
                                                "center", "dd", "div",
                                                "dl", "fieldset", "form",
                                                "head", "html",
                                                "iframe", "layer", "li",
                                                "marquee", "multicol",
                                                "noframes", "nolayer",
                                                "noscript", "object", "td",
                                                "th",
                                                NULL}
     }
    ,
    {"sound", TAG_SOUND, 0, 1, 0, 0, 1, 1, {    // 66 parents

                                            "a", "abbr", "address",
                                            "applet", "b",
                                            "bdo", "big", "blink",
                                            "blockquote", "body",
                                            "button", "caption", "center",
                                            "cite", "code",
                                            "dd", "dfn", "div", "dl", "dt",
                                            "em", "embed", "fieldset",
                                            "font", "form",
                                            "h1", "h2", "h3", "h4", "h5",
                                            "h6", "head", "html", "i",
                                            "iframe",
                                            "kbd", "label", "layer",
                                            "legend", "li",
                                            "marquee", "multicol", "nobr",
                                            "noembed", "noframes",
                                            "nolayer", "noscript",
                                            "object", "p", "pre",
                                            "q", "s", "samp", "small",
                                            "span",
                                            "strike", "strong", "sub",
                                            "sup", "table",
                                            "td", "th", "tr", "tt", "u",
                                            "var",
                                            NULL}
     }
    ,
    {"bgsound", TAG_BGSOUND, 0, 1, 0, 0, 1, 1, {        // 66 parents

                                                "a", "abbr", "address",
                                                "applet", "b",
                                                "bdo", "big", "blink",
                                                "blockquote", "body",
                                                "button", "caption",
                                                "center", "cite", "code",
                                                "dd", "dfn", "div", "dl",
                                                "dt",
                                                "em", "embed", "fieldset",
                                                "font", "form",
                                                "h1", "h2", "h3", "h4",
                                                "h5",
                                                "h6", "head", "html", "i",
                                                "iframe",
                                                "kbd", "label", "layer",
                                                "legend", "li",
                                                "marquee", "multicol",
                                                "nobr", "noembed",
                                                "noframes",
                                                "nolayer", "noscript",
                                                "object", "p", "pre",
                                                "q", "s", "samp", "small",
                                                "span",
                                                "strike", "strong", "sub",
                                                "sup", "table",
                                                "td", "th", "tr", "tt",
                                                "u",
                                                "var",
                                                NULL}
     }
    ,
    {"blink", TAG_BLINK, 0, 0, 1, 0, 1, 1, {    // 65 parents

                                            "a", "abbr", "address",
                                            "applet", "b",
                                            "bdo", "big", "blink",
                                            "blockquote", "body",
                                            "button", "caption", "center",
                                            "cite", "code",
                                            "dd", "dfn", "div", "dl", "dt",
                                            "em", "embed", "fieldset",
                                            "font", "form",
                                            "h1", "h2", "h3", "h4", "h5",
                                            "h6", "html", "i", "iframe",
                                            "kbd",
                                            "label", "layer", "legend",
                                            "li", "marquee",
                                            "multicol", "nobr", "noembed",
                                            "noframes", "nolayer",
                                            "noscript", "object", "p",
                                            "pre", "q",
                                            "s", "samp", "small", "span",
                                            "strike",
                                            "strong", "sub", "sup",
                                            "table", "td",
                                            "th", "tr", "tt", "u", "var",
                                            NULL}
     }
    ,
    {"comment", TAG_COMMENT, 0, 0, 1, 0, 0, 0, {        // 103 parents

                                                "a", "abbr", "acronym",
                                                "address", "applet",
                                                "area", "b", "base",
                                                "basefont", "bdo",
                                                "bgsound", "big", "blink",
                                                "blockquote", "body",
                                                "br", "button", "caption",
                                                "center", "cite",
                                                "code", "col", "colgroup",
                                                "dd", "del",
                                                "dfn", "dir", "div", "dl",
                                                "dt",
                                                "em", "embed", "fieldset",
                                                "font", "form",
                                                "frame", "frameset", "h1",
                                                "h2", "h3",
                                                "h4", "h5", "h6", "head",
                                                "hr",
                                                "html", "i", "iframe",
                                                "img", "input",
                                                "ins", "isindex", "kbd",
                                                "label", "layer",
                                                "legend", "li", "link",
                                                "map", "marquee",
                                                "menu", "meta", "multicol",
                                                "nobr", "noembed",
                                                "noframes", "nolayer",
                                                "noscript", "object", "ol",
                                                "optgroup", "option", "p",
                                                "param", "pre",
                                                "q", "s", "samp", "script",
                                                "select",
                                                "small", "sound", "spacer",
                                                "span", "strike",
                                                "strong", "style", "sub",
                                                "sup", "table",
                                                "tbody", "td", "textarea",
                                                "tfoot", "th",
                                                "thead", "title", "tr",
                                                "tt", "u",
                                                "ul", "var", "wbr",
                                                NULL}
     }
    ,
    {"multicol", TAG_MULTICOL, 0, 0, 1, 0, 1, 1, {      // 25 parents

                                                  "applet", "blockquote",
                                                  "body", "center", "dd",
                                                  "div", "dl", "embed",
                                                  "fieldset", "form",
                                                  "html", "iframe",
                                                  "layer", "li",
                                                  "multicol",
                                                  "noembed", "noframes",
                                                  "nolayer", "noscript",
                                                  "object",
                                                  "pre", "table", "td",
                                                  "th", "tr",
                                                  NULL}
     }
    ,
    {"nobr", TAG_NOBR, 0, 0, 1, 0, 0, 1, {      // 65 parents

                                          "a", "abbr", "address", "applet",
                                          "b",
                                          "bdo", "big", "blink",
                                          "blockquote", "body",
                                          "button", "caption", "center",
                                          "cite", "code",
                                          "dd", "dfn", "div", "dl", "dt",
                                          "em", "embed", "fieldset",
                                          "font", "form",
                                          "h1", "h2", "h3", "h4", "h5",
                                          "h6", "html", "i", "iframe",
                                          "kbd",
                                          "label", "layer", "legend", "li",
                                          "marquee",
                                          "multicol", "nobr", "noembed",
                                          "noframes", "nolayer",
                                          "noscript", "object", "p", "pre",
                                          "q",
                                          "s", "samp", "small", "span",
                                          "strike",
                                          "strong", "sub", "sup", "table",
                                          "td",
                                          "th", "tr", "tt", "u", "var",
                                          NULL}
     }
    ,
    {"noembed", TAG_NOEMBED, 0, 0, 1, 0, 1, 1, {        // 25 parents

                                                "applet", "blockquote",
                                                "body", "center", "dd",
                                                "div", "dl", "embed",
                                                "fieldset", "form",
                                                "html", "iframe", "layer",
                                                "li", "multicol",
                                                "noembed", "noframes",
                                                "nolayer", "noscript",
                                                "object",
                                                "pre", "table", "td", "th",
                                                "tr",
                                                NULL}
     }
    ,
    {"spacer", TAG_SPACER, 1, 1, 0, 0, 1, 1, {  // 65 parents

                                              "a", "abbr", "address",
                                              "applet", "b",
                                              "bdo", "big", "blink",
                                              "blockquote", "body",
                                              "button", "caption",
                                              "center", "cite", "code",
                                              "dd", "dfn", "div", "dl",
                                              "dt",
                                              "em", "embed", "fieldset",
                                              "font", "form",
                                              "h1", "h2", "h3", "h4", "h5",
                                              "h6", "html", "i", "iframe",
                                              "kbd",
                                              "label", "layer", "legend",
                                              "li", "marquee",
                                              "multicol", "nobr",
                                              "noembed", "noframes",
                                              "nolayer",
                                              "noscript", "object", "p",
                                              "pre", "q",
                                              "s", "samp", "small", "span",
                                              "strike",
                                              "strong", "sub", "sup",
                                              "table", "td",
                                              "th", "tr", "tt", "u", "var",
                                              NULL}
     }
    ,
    {"layer", TAG_LAYER, 0, 0, 1, 0, 1, 1, {    // 25 parents

                                            "applet", "blockquote", "body",
                                            "center", "dd",
                                            "div", "dl", "embed",
                                            "fieldset", "form",
                                            "html", "iframe", "layer",
                                            "li", "multicol",
                                            "noembed", "noframes",
                                            "nolayer", "noscript",
                                            "object",
                                            "pre", "table", "td", "th",
                                            "tr",
                                            NULL}
     }
    ,
    {"nolayer", TAG_NOLAYER, 0, 0, 1, 0, 1, 1, {        // 25 parents

                                                "applet", "blockquote",
                                                "body", "center", "dd",
                                                "div", "dl", "embed",
                                                "fieldset", "form",
                                                "html", "iframe", "layer",
                                                "li", "multicol",
                                                "noembed", "noframes",
                                                "nolayer", "noscript",
                                                "object",
                                                "pre", "table", "td", "th",
                                                "tr",
                                                NULL}
     }
    ,
    {"wbr", TAG_WBR, 0, 1, 0, 0, 0, 1, {        // 68 parents

                                        "a", "abbr", "acronym", "address",
                                        "applet",
                                        "b", "bdo", "big", "blink",
                                        "blockquote",
                                        "body", "button", "caption",
                                        "center", "cite",
                                        "code", "dd", "del", "dfn", "div",
                                        "dl", "dt", "em", "embed",
                                        "fieldset",
                                        "font", "form", "h1", "h2", "h3",
                                        "h4", "h5", "h6", "html", "i",
                                        "iframe", "ins", "kbd", "label",
                                        "layer",
                                        "legend", "li", "marquee",
                                        "multicol", "nobr",
                                        "noembed", "noframes", "nolayer",
                                        "noscript", "object",
                                        "p", "pre", "q", "s", "samp",
                                        "small", "span", "strike",
                                        "strong", "sub",
                                        "sup", "table", "td", "th", "tr",
                                        "tt", "u", "var",
                                        NULL}
     }
    ,
//total tag 104
};

int html_tag_info_size =
    sizeof(html_tag_info_array) / sizeof(html_tag_info_t);
