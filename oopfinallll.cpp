#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

//functions required:
// set current user
// view home
// like a post
// view liked list
// comment on a post
// view a post
// share memory
// view profile
// view friend list



// --- Utility Classes ---

// Date class
class Date {
private:
    int day, month, year;


public:
    static Date currentDate;
    Date(int d = 0, int m = 0, int y = 0) : day(d), month(m), year(y) {}

    static void setCurrentDate(int d, int m, int y) {
        currentDate.day = d;
        currentDate.month = m;
        currentDate.year = y;
    }

    static Date getCurrentDate() { return currentDate; }

    // this checks if the posts are recent ie. posted in the last 24 hours 


    bool isWithin24Hours() const {
        // convert both dates into "total days" for easy comparison
        int d1 = day + month * 30 + year * 365;
        int d2 = currentDate.day + currentDate.month * 30 + currentDate.year * 365;

        int diff = d2 - d1;

        return (diff >= 0 && diff <= 1);
    }

    void setDate(int d, int m, int y) {
        day = d;
        month = m;
        year = y;
    }

    void print() const { cout << day << "/" << month << "/" << year; }

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
};
//i hard coded the given date here
Date Date::currentDate(15, 11, 2017);

// Activity class
class Activity {
private:
    int type;
    string value;

public:
    Activity(int t = 0, string v = "") : type(t), value(v) {}
    // get t and v from the user and then output the feeling/thinking etc accordingly
    void print() const {
        if (type == 1) cout << " is feeling " << value;
        else if (type == 2) cout << " is thinking about " << value;
        else if (type == 3) cout << " is making " << value;
        else if (type == 4) cout << " is celebrating " << value;
    }

    int getType() const { return type; }
    string getValue() const { return value; }
};

// Forward declarations
class Object;
class User;
class Page;
class Comment;
class Post;

// Base Object class which will be used for posts comments users and pages
class Object {
protected:
    string id;

public:
    Object(string id = "") : id(id) {}
    virtual ~Object() {}

    string getId() const { return id; }
    virtual string getName() const = 0;
    virtual void printName() const = 0;
};


// Comment class
class Comment {
private:
    string id;
    string postId;
    Object* author;
    string text;

public:
    Comment() : author(nullptr) {}

    void setValues(string id, string postId, Object* auth, string text) {
        this->id = id;
        this->postId = postId;
        this->author = auth;
        this->text = text;
    }

    void print() const {
        if (author) {
            author->printName();
            cout << " wrote: \"" << text << "\"" << endl;
        }
    }

    string getText() const { return text; }
    Object* getAuthor() const { return author; }
};


// Post class
class Post {
private:
    string id;
    string text;
    Date date;
    Object* author;
    Activity* activity;
    Object** likedBy;
    Comment** comments;
    int likeCount;
    int commentCount;
    bool isMemory;
    Post* originalPost;
    string memoryText;

public:
    Post(string id = "", string txt = "", int d = 0, int m = 0, int y = 0)
        : id(id), text(txt), date(d, m, y), author(nullptr), activity(nullptr),
        likeCount(0), commentCount(0), isMemory(false), originalPost(nullptr) {
        likedBy = new Object * [10];
        comments = new Comment * [10];
        for (int i = 0; i < 10; i++) {
            likedBy[i] = nullptr;
            comments[i] = nullptr;
        }
    }

    ~Post() {
        delete[] likedBy;
        for (int i = 0; i < commentCount; i++) delete comments[i];
        delete[] comments;
        delete activity;
    }

    string getId() const { return id; }
    string getText() const { return text; }
    Date getDate() const { return date; }
    int getLikeCount() const { return likeCount; }

    void setAuthor(Object* auth) { author = auth; }
    void setActivity(int type, string value) { activity = new Activity(type, value); }

    void addLike(Object* obj) {
        if (likeCount < 10 && obj) {
            likedBy[likeCount++] = obj;
        }
    }

    void addComment(Comment* c) {
        if (commentCount < 10 && c) {
            comments[commentCount++] = c;
        }
    }

    void setAsMemory(Post* original, string memoryText) {
        isMemory = true;
        originalPost = original;
        this->memoryText = memoryText;
    }

    void displayPost() const {
        if (isMemory && originalPost) {
            cout << "~~~ " << author->getName() << " shared a memory ~~~" << endl;
            cout << "\"" << memoryText << "\"" << endl;
            cout << "(" << Date::getCurrentDate().getYear() - originalPost->date.getYear() << " Years Ago)" << endl;
            originalPost->displayPost();
            return;
        }

        if (activity) {
            author->printName();
            activity->print();
            cout << endl;
        }

        cout << "\"" << text << "\"";
        if (date.getDay() > 0) {
            cout << " ... (";
            date.print();
            cout << ")";
        }
        cout << endl;

        for (int i = 0; i < commentCount; i++) {
            cout << "    ";
            comments[i]->print();
        }
    }

    void displayLikedBy() const {
        cout << "Post Liked By:" << endl;
        for (int i = 0; i < likeCount; i++) {
            if (likedBy[i]) {
                cout << likedBy[i]->getId() << " -- " << likedBy[i]->getName() << endl;
            }
        }
    }

    bool isRecent() const { return date.isWithin24Hours(); }
};


// Page class
class Page : public Object {
    static const int MAX = 100;
private:
    string title;
    Post** timeline;
    int postCount;

public:
    Page(string id = "", string title = "") : Object(id), title(title), postCount(0) {
        timeline = new Post * [MAX];
        for (int i = 0; i < MAX; i++) timeline[i] = nullptr;
    }


    ~Page() {
        delete[] timeline;
    }
    string getName() const override { return title; }
    void printName() const override { cout << title; }

    void addPost(Post* post) {
        if (postCount < 100 && post) {
            timeline[postCount++] = post;
        }
    }

    Post** getTimeline() const { return timeline; }
    int getPostCount() const { return postCount; }
};
class User : public Object {
    string name;
    User** friends;
    Page** likedPages;
    Post** timeline;
    int friendCount, pageCount, postCount;
    static const int MAX = 100;
public:
    User(string id = "", string n = "") : Object(id), name(n), friendCount(0), pageCount(0), postCount(0) {
        friends = new User * [MAX];
        likedPages = new Page * [MAX];
        timeline = new Post * [MAX];
        for (int i = 0; i < MAX; i++) { friends[i] = nullptr; likedPages[i] = nullptr; timeline[i] = nullptr; }
    }

    ~User() {
        delete[] friends;
        delete[] likedPages;
        delete[] timeline;
    }

    string getName()const override { return name; }
    void printName() const override { cout << name; }

    //add functions
    void addFriend(User* u) { if (friendCount < MAX && u) friends[friendCount++] = u; }
    void likePage(Page* p) { if (pageCount < MAX && p) likedPages[pageCount++] = p; }
    void addPost(Post* p) { if (postCount < MAX) timeline[postCount++] = p; }
    User** getFriends() { return friends; }

    //getters
    int getFriendCount() const { return friendCount; }
    Page** getLikedPages() const { return likedPages; }
    int getPageCount() const { return pageCount; }
    Post** getTimeline() const { return timeline; }
    int getPostCount() const { return postCount; }

    //view functions
    void viewFriendList() const {
        cout << "\n--------------------------------------------------------------------------------\n";
        cout << name << " -- Friend List\n";
        for (int i = 0; i < friendCount; i++)
            cout << friends[i]->getId() << " - " << friends[i]->getName() << endl;
        cout << "--------------------------------------------------------------------------------\n";
    }

    void viewLikedPages() const {
        cout << "\n--------------------------------------------------------------------------------\n";
        cout << name << " -- Liked Pages\n";
        for (int i = 0; i < pageCount; i++)
            cout << likedPages[i]->getId() << " -- " << likedPages[i]->getName() << endl;
        cout << "--------------------------------------------------------------------------------\n";
    }

    void viewHome() const {
        cout << "\n--------------------------------------------------------------------------------\n";
        cout << name << " -- Home Page\n";
        cout << "--------------------------------------------------------------------------------\n";
        //friend posts
        for (int i = 0; i < friendCount; i++) {
            if (friends[i]) { //added this line here which makes sure that friends[i] is never nullptr
                for (int j = 0; j < friends[i]->getPostCount(); j++) {
                    Post* post = friends[i]->getTimeline()[j]; // made this change here

                    //using  post as  "friends[i]->getTimeline()[j]" is redundant and 
                    // also causes a problem when friends[i]->getTimeline()[j] is nullptr,
                    // so I have stored it in a variable and then checked if it's nullptr before accessing its members


                    if (post && post->getDate().isWithin24Hours()) {
                        cout << "--- " << friends[i]->getName()
                            << " shared \"" << post->getText() << "\"" << endl;

                        post->displayPost();
                        cout << endl;
                    }
                }
            }
        }
        //page posts
        for (int i = 0; i < pageCount; i++) {
            if (likedPages[i]) { //added this line here which makes sure that likedPages[i] is never nullptr

                Post** posts = likedPages[i]->getTimeline();

                for (int j = 0; j < likedPages[i]->getPostCount(); j++) {
                    Post* post = posts[j];

                    if (post && post->getDate().isWithin24Hours()) {
                        post->displayPost();
                        cout << endl;
                    }
                }
            }
            cout << "--------------------------------------------------------------------------------\n";
        }
    }

    void viewTimeline() const {
        cout << "\n--------------------------------------------------------------------------------\n";
        cout << name << " -- Time Line\n";
        cout << "--------------------------------------------------------------------------------\n";
        for (int i = postCount - 1; i >= 0; i--) {
            if (timeline[i]) {
                timeline[i]->displayPost();
                cout << endl;
            }
        }
        cout << "--------------------------------------------------------------------------------\n";
    }
};

// SocialNetworkApp class to manage everything

class SocialNetworkApp {
    User** allUsers;
    Page** allPages;
    Post** allPosts;
    int totalUsers, totalPages, totalPosts;
    User* currentUser;
    static const int MAX = 100;

public:
    SocialNetworkApp() : allUsers(nullptr), allPages(nullptr), allPosts(nullptr),
        totalUsers(0), totalPages(0), totalPosts(0), currentUser(nullptr) {
    }

    ~SocialNetworkApp() {
        for (int i = 0; i < totalUsers; i++) delete allUsers[i];
        delete[] allUsers;
        for (int i = 0; i < totalPages; i++) delete allPages[i];
        delete[] allPages;
        for (int i = 0; i < totalPosts; i++) delete allPosts[i];
        delete[] allPosts;
    }

    User* GetUserByID(string id) {
        for (int i = 0; i < totalUsers; i++)
            if (allUsers[i]->getId() == id) return allUsers[i];
        return nullptr;
    }

    Page* GetPageByID(string id) {
        for (int i = 0; i < totalPages; i++)
            if (allPages[i]->getId() == id) return allPages[i];
        return nullptr;
    }

    Object* GetObjectByID(string id) {
        if (id.empty()) return nullptr;
        if (id[0] == 'u') return GetUserByID(id);
        if (id[0] == 'p') return GetPageByID(id);
        return nullptr;
    }

    Post* GetPostByID(string id) {
        for (int i = 0; i < totalPosts; i++)
            if (allPosts[i]->getId() == id) return allPosts[i];
        return nullptr;
    }

    void LoadData() {
        // Load Pages
        ifstream fPages("Pages.txt");
        if (!fPages) { cout << "Error: Pages.txt not found!\n"; return; }
        string line;
        getline(fPages, line);
        totalPages = stoi(line);
        allPages = new Page * [totalPages];
        for (int i = 0; i < totalPages; i++) {
            getline(fPages, line);
            if (line.empty()) getline(fPages, line);
            stringstream ss(line);
            string id, title;
            ss >> id;
            getline(ss, title);
            if (!title.empty() && title[0] == ' ') title = title.substr(1);
            allPages[i] = new Page(id, title);
        }
        fPages.close();

        // Load Users
        ifstream fUsers("Users.txt");
        if (!fUsers) { cout << "Error: Users.txt not found!\n"; return; }
        getline(fUsers, line);
        totalUsers = stoi(line);
        allUsers = new User * [totalUsers];
        for (int i = 0; i < totalUsers; i++) {
            getline(fUsers, line);
            if (line.empty()) getline(fUsers, line);
            stringstream ss(line);
            string id, name;
            ss >> id >> name;
            allUsers[i] = new User(id, name);
        }
        fUsers.close();

        // Load Posts first pass - create posts
        ifstream fPosts("Posts.txt");
        if (!fPosts) { cout << "Error: Posts.txt not found!\n"; return; }
        getline(fPosts, line);
        totalPosts = stoi(line);
        allPosts = new Post * [totalPosts];

        for (int i = 0; i < totalPosts; i++) {
            getline(fPosts, line);
            while (line.empty() && !fPosts.eof()) getline(fPosts, line);

            stringstream ss(line);
            int type;
            string pid, owner;
            ss >> type >> pid >> owner;

            getline(fPosts, line);
            while (line.empty()) getline(fPosts, line);
            int d, m, y;
            stringstream ds(line);
            ds >> d >> m >> y;

            getline(fPosts, line);
            while (line.empty()) getline(fPosts, line);
            string text = line;

            allPosts[i] = new Post(pid, text, d, m, y);

            if (type > 1) {
                getline(fPosts, line);
                while (line.empty()) getline(fPosts, line);
                int atype;
                string aval;
                stringstream as(line);
                as >> atype;
                getline(as, aval);
                if (!aval.empty() && aval[0] == ' ') aval = aval.substr(1);
                allPosts[i]->setActivity(atype, aval);
            }
            else {
                getline(fPosts, line);
            }

            getline(fPosts, line);
        }
        fPosts.close();

        // Second pass - link posts to authors and likes
        ifstream fPosts2("Posts.txt");
        getline(fPosts2, line);
        for (int i = 0; i < totalPosts; i++) {
            getline(fPosts2, line);
            while (line.empty()) getline(fPosts2, line);

            stringstream ss(line);
            int type;
            string pid, owner;
            ss >> type >> pid >> owner;

            for (int j = 0; j < 4; j++) getline(fPosts2, line);

            getline(fPosts2, line);
            stringstream ls(line);
            string token;
            while (ls >> token && token != "-1") {
                Object* obj = GetObjectByID(token);
                if (obj) allPosts[i]->addLike(obj);
            }

            Object* auth = GetObjectByID(owner);
            allPosts[i]->setAuthor(auth);
            if (owner[0] == 'u') {
                User* u = GetUserByID(owner);
                if (u) u->addPost(allPosts[i]);
            }
            else {
                Page* p = GetPageByID(owner);
                if (p) p->addPost(allPosts[i]);
            }
        }
        fPosts2.close();

        // Load Comments
        ifstream fComm("Comments.txt");
        if (fComm) {
            getline(fComm, line);
            int tC = stoi(line);
            for (int i = 0; i < tC; i++) {
                getline(fComm, line);
                if (line.empty()) getline(fComm, line);
                stringstream ss(line);
                string cid, pid, uid, text;
                ss >> cid >> pid >> uid;
                getline(ss, text);
                if (!text.empty() && text[0] == ' ') text = text.substr(1);

                Comment* c = new Comment();
                c->setValues(cid, pid, GetObjectByID(uid), text);
                Post* p = GetPostByID(pid);
                if (p) p->addComment(c);
            }
            fComm.close();
        }

        // Link friends and liked pages
        ifstream fUsers2("Users.txt");
        getline(fUsers2, line);
        for (int i = 0; i < totalUsers; i++) {
            getline(fUsers2, line);
            if (line.empty()) getline(fUsers2, line);
            stringstream ss(line);
            string id, name, token;
            ss >> id >> name;

            // Expecting friends list until -1 then liked pages, read remaining tokens
            bool readingFriends = true;
            while (ss >> token) {
                if (token == "-1") {
                    readingFriends = false;
                    continue;
                }
                if (readingFriends && token.size() > 0 && token[0] == 'u') {
                    User* u = GetUserByID(token);
                    if (u) allUsers[i]->addFriend(u);
                }
                else if (!readingFriends && token.size() > 0 && token[0] == 'p') {
                    Page* p = GetPageByID(token);
                    if (p) allUsers[i]->likePage(p);
                }
            }
        }
        fUsers2.close();
    }

private:
    // helper to read a line from stdin
    string ReadLine(const string& prompt) {
        cout << prompt;
        string s;
        getline(cin, s);
        return s;
    }

    // Add a post to system arrays (realloc-like)
    void AddPostToSystem(Post* p) {
        Post** temp = new Post * [totalPosts + 1];
        for (int i = 0; i < totalPosts; i++) temp[i] = allPosts[i];
        temp[totalPosts] = p;
        delete[] allPosts;
        allPosts = temp;
        totalPosts++;
    }

    void SetCurrentUser() {
        string uid = ReadLine("Enter User ID (e.g. u7): ");
        if (uid.empty()) { cout << "No ID entered.\n"; return; }
        User* u = GetUserByID(uid);
        if (u) {
            currentUser = u;
            cout << currentUser->getName() << " successfully set as Current User\n";
        }
        else {
            cout << "User not found: " << uid << endl;
        }
    }

    void SetSystemDate() {
        string line = ReadLine("Enter date (D M Y): ");
        stringstream ss(line);
        int d, m, y;
        if (!(ss >> d >> m >> y)) { cout << "Invalid date format.\n"; return; }
        Date::currentDate.setDate(d, m, y);
        cout << "System Date: " << d << "/" << m << "/" << y << endl;
    }

    void ViewHome() {
        if (!currentUser) { cout << "No current user set.\n"; return; }
        currentUser->viewHome();
    }

    void LikePost() {
        if (!currentUser) { cout << "No current user set.\n"; return; }
        string pid = ReadLine("Enter Post ID to like: ");
        Post* p = GetPostByID(pid);
        if (!p) { cout << "Post not found: " << pid << endl; return; }
        p->addLike(currentUser);
        cout << "Post liked successfully!\n";
    }

    void ViewLikedList() {
        string pid = ReadLine("Enter Post ID to view liked list: ");
        Post* p = GetPostByID(pid);
        if (!p) { cout << "Post not found: " << pid << endl; return; }
        cout << "\n--------------------------------------------------------------------------------\n";
        p->displayLikedBy();
        cout << "--------------------------------------------------------------------------------\n";
    }

    void PostComment() {
        if (!currentUser) { cout << "No current user set.\n"; return; }
        string pid = ReadLine("Enter Post ID to comment on: ");
        Post* p = GetPostByID(pid);
        if (!p) { cout << "Post not found: " << pid << endl; return; }
        string text = ReadLine("Enter comment text: ");
        Comment* c = new Comment();
        c->setValues("", pid, currentUser, text);
        p->addComment(c);
        cout << "Comment added successfully!\n";
    }

    void ViewPost() {
        string pid = ReadLine("Enter Post ID to view: ");
        Post* p = GetPostByID(pid);
        if (!p) { cout << "Post not found: " << pid << endl; return; }
        cout << "\n--------------------------------------------------------------------------------\n";
        p->displayPost();
        cout << "--------------------------------------------------------------------------------\n";
    }

    void ShareMemory() {
        if (!currentUser) { cout << "No current user set.\n"; return; }
        string pid = ReadLine("Enter Post ID to create memory from: ");
        Post* orig = GetPostByID(pid);
        if (!orig) { cout << "Original post not found: " << pid << endl; return; }
        string memText = ReadLine("Enter memory text: ");
        // create new post as memory
        string newId = "mem" + to_string(totalPosts + 1);
        Date cd = Date::getCurrentDate();
        Post* mem = new Post(newId, "", cd.getDay(), cd.getMonth(), cd.getYear());
        mem->setAuthor(currentUser);
        mem->setAsMemory(orig, memText);
        // add to user's timeline and system posts
        currentUser->addPost(mem);
        AddPostToSystem(mem);
        cout << "Memory shared successfully!\n";
    }

    void ViewProfile() {
        if (!currentUser) { cout << "No current user set.\n"; return; }
        currentUser->viewTimeline();
    }

    void ViewFriendList() {
        string uid = ReadLine("Enter User ID to view friend list (or press enter for current user): ");
        if (uid.empty()) {
            if (!currentUser) { cout << "No current user set.\n"; return; }
            currentUser->viewFriendList();
            return;
        }
        User* u = GetUserByID(uid);
        if (!u) { cout << "User not found: " << uid << endl; return; }
        u->viewFriendList();
    }

    void ViewPage() {
        string pid = ReadLine("Enter Page ID to view (e.g. p1): ");
        Page* page = GetPageByID(pid);
        if (!page) { cout << "Page not found: " << pid << endl; return; }
        cout << "\n--------------------------------------------------------------------------------\n";
        cout << page->getName() << endl;
        cout << "--------------------------------------------------------------------------------\n";
        for (int i = page->getPostCount() - 1; i >= 0; i--) {
            if (page->getTimeline()[i]) {
                page->getTimeline()[i]->displayPost();
                cout << endl;
            }
        }
        cout << "--------------------------------------------------------------------------------\n";
    }

    void ShowMenu() {
        cout << "\n================ Social Network Menu ================\n";
        cout << "1. Set current System Date\n";
        cout << "2. Set Current User\n";
        cout << "3. View Home\n";
        cout << "4. Like a Post\n";
        cout << "5. View Liked List for a Post\n";
        cout << "6. Post a Comment\n";
        cout << "7. View a Post\n";
        cout << "8. Share a Memory\n";
        cout << "9. View My Profile (Timeline)\n";
        cout << "10. View Friend List\n";
        cout << "11. View Page\n";
        cout << "0. Exit\n";
        cout << "=====================================================\n";
    }

public:
    void Run() {
        LoadData();

        while (true) {
            ShowMenu();
            string choiceStr = ReadLine("Choose option: ");
            if (choiceStr.empty()) continue;
            int choice = -1;
            try { choice = stoi(choiceStr); }
            catch (...) { cout << "Invalid choice\n"; continue; }

            switch (choice) {
            case 0: return;
            case 1: SetSystemDate(); break;
            case 2: SetCurrentUser(); break;
            case 3: ViewHome(); break;
            case 4: LikePost(); break;
            case 5: ViewLikedList(); break;
            case 6: PostComment(); break;
            case 7: ViewPost(); break;
            case 8: ShareMemory(); break;
            case 9: ViewProfile(); break;
            case 10: ViewFriendList(); break;
            case 11: ViewPage(); break;
            default: cout << "Unknown option\n"; break;
            }
        }
    }
};

int main() {
    SocialNetworkApp app;
    app.Run();
    cout << "hello";
    return 0;
}
