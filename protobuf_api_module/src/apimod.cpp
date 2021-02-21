#include <protobufapimod/apimod.hpp>



proto::proto(std::string output) {
    output = output;
}

void proto::set_frame(Frame frame, DetectedObject obj) {
    auto rect = frame.mutable_rect_inf();
    auto loct = frame.mutable_location_inf();
    rect->set_height(obj.bounding_box.height);
    rect->set_width(obj.bounding_box.width);
    rect->set_x(obj.bounding_box.x);
    rect->set_y(obj.bounding_box.y);

    loct->set_x(obj.location.x);
    loct->set_y(obj.location.y);
    loct->set_z(obj.location.z);

    frame.set_id(obj.id);
    frame.set_name(obj.name);
    frame.set_distance(obj.distance);
}

void proto::send(DetectedObjects objects, const char * pipe) {
    Frame frame;
    int fd = open(pipe,O_WRONLY);

    frame.Clear();
    for(const DetectedObject &obj: objects) {
        this->set_frame(frame, obj);
        int header = frame.ByteSizeLong();
        frame.SerializeToString(&this->output);
        write(fd, &header, 4);
        frame.SerializeToFileDescriptor(fd);
        }

    }



