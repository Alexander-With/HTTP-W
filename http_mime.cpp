#include "http_mime.h"

std::string getmime(const std::string &s)
{
    size_t i = s.rfind('.', s.length());
    if (i != std::string::npos)
    {
        std::string ext = s.substr(i + 1, s.length() - i);
        // Text
        if (ext == "txt")
            return "text/plain";
        else if (ext == "html")
            return "text/html";
        else if (ext == "css")
            return "text/css";
        else if (ext == "js")
            return "text/javascript";
        // Images
        else if (ext == "gif")
            return "image/gif";
        else if (ext == "jpg")
            return "image/jpeg";
        else if (ext == "png")
            return "image/png";
        else if (ext == "webp")
            return "image/webp";
        else if (ext == "svg")
            return "image/svg+xml";
        else if (ext == "ico")
            return "image/x-icon";
        // Audio
        else if (ext == "wav")
            return "audio/wave";
        else if (ext == "ogg")
            return "audio/ogg";
        else if (ext == "mp3")
            return "audio/mpeg";
        // Video
        else if (ext == "webm")
            return "video/webm";
        else if (ext == "mp4")
            return "video/mp4";
        else
            return "application/octet-stream";
    }
    return "application/octet-stream";
}