#pragma once

#include "taglib_config.h"
#include "tfile.h"
#include "fileref.h"
#include "tag.h"
#include "tpropertymap.h"
#include "id3v2tag.h"
#include "id3v2frame.h"
#include "id3v2header.h"
#include "mpegfile.h"
#include "attachedpictureframe.h"
#include "unsynchronizedlyricsframe.h"
#include "synchronizedlyricsframe.h"

class ImageFile : public TagLib::File {
public:
    static ImageFile *getImageFile(std::string img);
    void setArt(std::string song);

private:
    static ImageFile *single;
    ImageFile(std::string img);

protected:
    virtual TagLib::Tag *tag() const;
    virtual TagLib::AudioProperties *audioProperties() const;
    virtual bool save();
};
