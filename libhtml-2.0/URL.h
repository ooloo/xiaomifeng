#ifndef URL_H
#define URL_H
#include <stdint.h>
#include <string.h>


class URL {
  public:
    URL();
    URL(const char *);
     URL(const char *, bool hasDepth);
     URL(const URL & url);
    ~URL();
    char *getURL() const {
        return url;
    }
    bool setURL(const char *);

    bool isValid();
    bool isAbsoluteURL();

    void setDepth(short d) {
        depth = d;
    } void setBaseURL(const char *str);
    void getURLPrefix(char **p_str, int len);
    char *getScheme() const {
        return scheme;
    }
    char *getHost() const {
        return host;
    }
    char *getPort() const {
        return port;
    }
    short getDepth() {
        return depth;
    }
    char *getPath() const {
        return path;
    }
    unsigned long getHashCode();
/*
        uint32_t getHostID() { return getFingerPrint(host); }
        uint32_t getURLID() { return getFingerPrint(url); }
        uint64_t getDOCID() { uint64_t DOCID=getHostID(); DOCID=DOCID<<32; DOCID|=getURLID(); return DOCID;}    
*/
    bool isPath();
    bool isIndexFile();


    void getCannonURL(char *buffer);

  private:
    void clean();
    void scan();
    void simplifyURL();
    char *URLCanon(char *p_host);
//        uint32_t getFingerPrint(const char* s); 
  private:
    char *url;
    char *scheme;               // protocol
    char *host;                 // server name
    char *port;                 // http server port
    char *path;                 // absolute path of the file
    short depth;

    int port_val;

};

struct compareURL {
    bool operator() (URL * u1, URL * u2) const {
        return (u1->getDepth() > u2->getDepth());
    }
};
#endif
