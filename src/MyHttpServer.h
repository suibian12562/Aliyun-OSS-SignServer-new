#ifndef MyHttpServer_h
#define MyHttpServer_h

using Poco::Util::OptionSet;


class MyHttpServer : public Poco::Util::ServerApplication
{
protected:
    void initialize(Application &self);

    void uninitialize();

    void defineOptions(OptionSet &options);

    void displayHelp();

    int main(const std::vector<std::string> &args);
};

#endif
