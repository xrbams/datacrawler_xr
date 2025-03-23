// #include "scheduler.hpp"

// using namespace std;


// int main(int argc, char const *argv[]){
//     std::string robo =
//         "User-Agent: *\n"
//         "Disallow: /private/\n"
//         "Disallow: /admin/\n"
//         "Disallow: /hidden/\n"
//         "Disallow: /feed/\n"
//         "Disallow: /playlist\n"
//         "Disallow: /channel/\n"
//         "\n"
//         "User-Agent: TestBot\n"
//         "Disallow: /test-area/\n"
//         "Disallow: /no-access/\n";

//     std::vector<std::string> links = {
//         "https://www.youtube.com/feed/trending/feed/",
//         "https://www.youtube.com/playlist?list=PL123456/playlist",
//         "https://www.youtube.com/channel/UC123456/videos/channel/",
//         "https://www.youtube.com/watch?v=dQw4w9WgXcQ/wabo/erfg/wsdf",
//         "https://www.youtube.com/results?search_query=robotics"
//     };


//     Robot roberto;
//     TokenBucket limit(5);
//     DepthTracker tracker(5);
//     roberto.parser(robo);
//     for (const auto &i : links){
//         if(!roberto.isAllowed(i)){
//             cout << "Blocked by robots: " << i << endl;
//             continue;
//         }

//         if(!tracker.canCrawl(i)){
//             std::cout << "Checking URL: " << i << " Depth: " << tracker.getDepth(i) << std::endl;
//             continue;
//         }

//         // Rate limiting
//         limit.waitForToken();
//     }
    
//     return 0;
// }
