#include "img.h"

ImageFile *ImageFile::getImageFile(std::string img) {
    if (single == NULL)
        single = new ImageFile(img);
    return single;
};

void ImageFile::setArt(std::string song) {
    TagLib::MPEG::File audioFile(song.c_str());
    TagLib::ID3v2::Tag *tag = audioFile.ID3v2Tag(true);
    TagLib::ID3v2::AttachedPictureFrame *frame = new TagLib::ID3v2::AttachedPictureFrame;

    if (!tag->frameListMap()["APIC"].isEmpty())
        tag->removeFrames(tag->frameListMap()["APIC"].front()->frameID());
    frame->setMimeType("image/jpeg");
    frame->setPicture(readBlock(length()));
    tag->addFrame(frame);
    audioFile.save();
};

ImageFile *ImageFile::single = NULL;

ImageFile::ImageFile(std::string img) : TagLib::File(img.c_str()) {

};

TagLib::Tag *ImageFile::tag() const {
    return 0;
};

TagLib::AudioProperties *ImageFile::audioProperties() const {
    return 0;
};

bool ImageFile::save() {
    return false;
};
