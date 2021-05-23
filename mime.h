#ifndef MIME_H
#define MIME_h

static const char APP_JSON[] PROGMEM = "application/json";
static const char APP_GZIP[] PROGMEM = "application/gzip";
static const char APP_JS[] PROGMEM = "application/javascript";
static const char APP_EOT[] PROGMEM = "application/vnd.ms-fontobject";

static const char IMG_ICO[] PROGMEM = "image/x-icon";
static const char IMG_SVG[] PROGMEM = "image/svg+xml";

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char TEXT_HTML[] PROGMEM = "text/html";
static const char TEXT_CSS[] PROGMEM = "text/css";
static const char TEXT_XML[] PROGMEM = "text/xml";

static const char FONT_TTF[] PROGMEM = "font/ttf";
static const char FONT_WOF1[] PROGMEM = "font/woff";
static const char FONT_WOF2[] PROGMEM = "font/woff2";

// convert the file extension to the MIME type
String getContentType(String filename) {
    filename.toUpperCase();

    if (filename.endsWith(".HTML"))
        return FPSTR(TEXT_HTML);
    else if (filename.endsWith(".CSS"))
        return FPSTR(TEXT_CSS);
    else if (filename.endsWith(".GZ"))
        return FPSTR(APP_GZIP);
    else if (filename.endsWith(".ICO"))
        return FPSTR(IMG_ICO);
    else if (filename.endsWith(".JS"))
        return FPSTR(APP_JS);
    else if (filename.endsWith(".JSON"))
        return FPSTR(APP_JSON);
    else if (filename.endsWith(".TXT"))
        return FPSTR(TEXT_PLAIN);
    else if (filename.endsWith(".EOT"))
        return FPSTR(APP_EOT);
    else if (filename.endsWith(".SVG"))
        return FPSTR(IMG_SVG);
    else if (filename.endsWith(".TTF"))
        return FPSTR(FONT_TTF);
    else if (filename.endsWith(".WOFF"))
        return FPSTR(FONT_WOF1);
    else if (filename.endsWith(".WOFF2"))
        return FPSTR(FONT_WOF2);
    else if (filename.endsWith(".XML"))
        return FPSTR(TEXT_XML);
    return FPSTR(TEXT_PLAIN);
}

#endif
