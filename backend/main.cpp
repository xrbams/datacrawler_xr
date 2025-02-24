#include "modules/module_a/downloader.cpp"

using namespace std;

int main(int argc, char const *argv[])
{
    cout << "This is my first go at Concurrent Web Crawler" << endl;
    Downloader d("https://www.google.com");

    if(d.fetch()){
        cout << "URL: " << d.getUrl() << endl;
        cout << "Status: " << d.getStatusCode() << endl;
        cout << "Crawled At: " << d.getTimestamp() << endl;
    }
    //getstorage();
    return 0;
}
