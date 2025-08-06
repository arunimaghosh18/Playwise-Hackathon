#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stack>
#include <queue>
#include <functional>
#include <cctype>
#include <memory>
#include <map>

using namespace std;


// ================= Song Metadata =================
class Song {
public:
    int song_id;
    string title;
    string artist;
    int duration; // seconds
    int rating;   // 1-5 or 0/unrated
    int total_listen_time;

    Song(int id, string t, string a, int d)
        : song_id(id), title(t), artist(a), duration(d),
          rating(0), total_listen_time(0) {}

    string display() const {
        return title + " by " + artist + " (" + to_string(duration) + " sec)";
    }
};

// ================= Playlist (Doubly Linked List) =================
struct PlaylistNode {
    shared_ptr<Song> song;
    PlaylistNode* prev;
    PlaylistNode* next;
    PlaylistNode(shared_ptr<Song> s) : song(s), prev(nullptr), next(nullptr) {}
};

class Playlist {
private:
    PlaylistNode* head;
    PlaylistNode* tail;
    int length;

public:
    Playlist() : head(nullptr), tail(nullptr), length(0) {}
    ~Playlist() {
        while (head) {
            PlaylistNode* next = head->next;
            delete head;
            head = next;
        }
    }

    bool exists(const string& title, const string& artist) const {
        string t = title, a = artist;
        transform(t.begin(), t.end(), t.begin(), ::tolower);
        transform(a.begin(), a.end(), a.begin(), ::tolower);
        PlaylistNode* curr = head;
        while (curr) {
            string ct = curr->song->title, ca = curr->song->artist;
            transform(ct.begin(), ct.end(), ct.begin(), ::tolower);
            transform(ca.begin(), ca.end(), ca.begin(), ::tolower);
            if (ct == t && ca == a) return true;
            curr = curr->next;
        }
        return false;
    }

    int size() const { return length; }

    void add_song(shared_ptr<Song> song) {
        PlaylistNode* node = new PlaylistNode(song);
        if (!head) head = tail = node;
        else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        length++;
    }

    void delete_song(int idx, function<void(shared_ptr<Song>)> remove_from_lookup) {
        if (idx < 0 || idx >= length) {
            cout << "\n[ERROR] Invalid index. No song deleted.\n";
            return;
        }
        PlaylistNode* curr = head;
        for (int i = 0; i < idx; ++i) curr = curr->next;

        remove_from_lookup(curr->song);

        if (curr->prev) curr->prev->next = curr->next;
        else head = curr->next;
        if (curr->next) curr->next->prev = curr->prev;
        else tail = curr->prev;
        delete curr;
        length--;
        cout << "\n[INFO] Song deleted successfully.\n";
    }

    // New swap-based move_song
    void move_song(int from_idx, int to_idx) {
        if (from_idx < 0 || from_idx >= length || to_idx < 0 || to_idx >= length) {
            cout << "\n[ERROR] Invalid index. Move operation aborted.\n";
            return;
        }
        if (from_idx == to_idx) {
            cout << "\n[INFO] No movement needed (same index).\n";
            return;
        }

        PlaylistNode* node1 = head;
        for (int i = 0; i < from_idx; ++i) node1 = node1->next;

        PlaylistNode* node2 = head;
        for (int i = 0; i < to_idx; ++i) node2 = node2->next;

        swap(node1->song, node2->song);
        cout << "\n[INFO] Songs swapped successfully.\n";
    }

    void reverse_playlist() {
        PlaylistNode* curr = head;
        while (curr) {
            swap(curr->next, curr->prev);
            curr = curr->prev;
        }
        swap(head, tail);
        cout << "\n[INFO] Playlist reversed successfully.\n";
    }

    void show() const {
        cout << "\n===========================================\n";
        cout << "             CURRENT PLAYLIST\n";
        cout << "===========================================\n";
        if (!head) {
            cout << "[EMPTY] No songs in playlist.\n";
        } else {
            PlaylistNode* curr = head;
            int idx = 0;
            while (curr) {
                cout << idx << ". " << curr->song->display() << endl;
                curr = curr->next;
                idx++;
            }
        }
        cout << "===========================================\n";
    }

    vector<shared_ptr<Song>> all_songs() const {
        vector<shared_ptr<Song>> v;
        PlaylistNode* curr = head;
        while (curr) { v.push_back(curr->song); curr = curr->next; }
        return v;
    }
};

// ================= Song Lookup (HashMap) =================
class SongLookup {
    unordered_map<int, shared_ptr<Song>> map_by_id;
    unordered_map<string, shared_ptr<Song>> map_by_title;

    static string to_lower(const string& s) {
        string result = s;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
public:
    void add_song(shared_ptr<Song> s) {
        map_by_id[s->song_id] = s;
        map_by_title[to_lower(s->title)] = s;
    }
    void remove_song(shared_ptr<Song> s) {
        map_by_id.erase(s->song_id);
        map_by_title.erase(to_lower(s->title));
    }

    shared_ptr<Song> get_by_title(const string& title) {
        auto it = map_by_title.find(to_lower(title));
        return it != map_by_title.end() ? it->second : nullptr;
    }
    
    vector<shared_ptr<Song>> search_by_partial_title(const string& term) {
        vector<shared_ptr<Song>> results;
        string t = to_lower(term);
        for (auto& kv : map_by_title) {
            if (kv.first.find(t) != string::npos) {
                results.push_back(kv.second);
            }
        }
        return results;
    }
};

// ================= Playback History (Stack) =================
class PlaybackHistory {
    stack<shared_ptr<Song>> history;
public:
    void play(shared_ptr<Song> song) { history.push(song); }
    shared_ptr<Song> undo_last_play() {
        if (history.empty()) return nullptr;
        shared_ptr<Song> s = history.top(); history.pop();
        return s;
    }
    vector<shared_ptr<Song>> recent(int n) {
        vector<shared_ptr<Song>> out;
        stack<shared_ptr<Song>> temp = history;
        while (!temp.empty() && n--) {
            out.push_back(temp.top());
            temp.pop();
        }
        return out;
    }
};

// ================= Song Rating Tree (BST) =================
struct RatingNode {
    int rating;
    vector<shared_ptr<Song>> songs;
    RatingNode* left;
    RatingNode* right;
    RatingNode(int r) : rating(r), left(nullptr), right(nullptr) {}
};

class SongRatingBST {
    RatingNode* root;

    RatingNode* _insert(RatingNode* node, shared_ptr<Song> s, int r) {
        if (!node) {
            node = new RatingNode(r);
            node->songs.push_back(s);
            return node;
        }
        if (r < node->rating) node->left = _insert(node->left, s, r);
        else if (r > node->rating) node->right = _insert(node->right, s, r);
        else node->songs.push_back(s);
        return node;
    }

    RatingNode* _delete_song(RatingNode* node, int id) {
        if (!node) return nullptr;
        node->songs.erase(remove_if(node->songs.begin(), node->songs.end(),
                                    [id](auto s){ return s->song_id == id; }),
                          node->songs.end());
        if (node->songs.empty()) {
            if (!node->left) {
                RatingNode* r = node->right;
                delete node;
                return r;
            } else if (!node->right) {
                RatingNode* l = node->left;
                delete node;
                return l;
            }
        }
        node->left = _delete_song(node->left, id);
        node->right = _delete_song(node->right, id);
        return node;
    }

    void _count_by_rating(RatingNode* node, map<int,int>& countMap) {
        if (!node) return;
        countMap[node->rating] += node->songs.size();
        _count_by_rating(node->left, countMap);
        _count_by_rating(node->right, countMap);
    }

    void _free(RatingNode* node) {
        if (!node) return;
        _free(node->left); _free(node->right); delete node;
    }

public:
    SongRatingBST() : root(nullptr) {}
    ~SongRatingBST() { _free(root); }

    void insert_or_update(shared_ptr<Song> song, int r) {
        if (song->rating != 0) {
            root = _delete_song(root, song->song_id);
        }
        song->rating = r;
        root = _insert(root, song, r);
    }
    void delete_song(int song_id) {
        root = _delete_song(root, song_id);
    }
    map<int,int> rating_counts() {
        map<int,int> counts;
        _count_by_rating(root, counts);
        return counts;
    }
};

// ================= Favorites (Max Heap) =================
struct HeapItem {
    int listen_time;
    shared_ptr<Song> song;
    bool operator<(const HeapItem& other) const {
        return listen_time < other.listen_time;
    }
};

class FavoriteQueue {
    priority_queue<HeapItem> heap;
    unordered_map<int, int> listen_map;
public:
    void add_or_update(shared_ptr<Song> s) {
        listen_map[s->song_id] = s->total_listen_time;
        heap.push({s->total_listen_time, s});
    }
    vector<shared_ptr<Song>> get_top_favorites(int k = 5) {
        vector<HeapItem> popped;
        vector<shared_ptr<Song>> out;
        int count = 0;
        while (!heap.empty() && count < k) {
            HeapItem top = heap.top(); heap.pop();
            if (listen_map[top.song->song_id] == top.listen_time) {
                out.push_back(top.song); count++;
            }
            popped.push_back(top);
        }
        for (auto& item : popped)
            heap.push(item);
        return out;
    }
};

// ================= Utility Functions =================
vector<shared_ptr<Song>> quick_sort_songs(
    const vector<shared_ptr<Song>>& songs,
    function<bool(shared_ptr<Song>, shared_ptr<Song>)> cmp) 
{
    if (songs.size() <= 1) return songs;
    auto pivot = songs[0];
    vector<shared_ptr<Song>> less, more;
    for (size_t i = 1; i < songs.size(); ++i) {
        if (cmp(songs[i], pivot))
            less.push_back(songs[i]);
        else
            more.push_back(songs[i]);
    }
    auto sorted_less = quick_sort_songs(less, cmp);
    auto sorted_more = quick_sort_songs(more, cmp);
    sorted_less.push_back(pivot);
    sorted_less.insert(sorted_less.end(), sorted_more.begin(), sorted_more.end());
    return sorted_less;
}

void playlist_duration_summary(const Playlist& playlist) {
    auto songs = playlist.all_songs();
    if (songs.empty()) {
        cout << "[EMPTY] Playlist is empty.\n";
        return;
    }
    int total = 0;
    auto longest = songs[0], shortest = songs[0];
    for (auto& s : songs) {
        total += s->duration;
        if (s->duration > longest->duration) longest = s;
        if (s->duration < shortest->duration) shortest = s;
    }
    cout << "Total Playtime: " << total << " sec\n";
    cout << "Longest Song: " << longest->display() << "\n";
    cout << "Shortest Song: " << shortest->display() << "\n";
}

unordered_set<string> blocked_artists;
bool is_blocked_artist(const string& artist) {
    string a = artist;
    transform(a.begin(), a.end(), a.begin(), ::tolower);
    return blocked_artists.count(a);
}
void block_artist(const string& artist) {
    string a = artist;
    transform(a.begin(), a.end(), a.begin(), ::tolower);
    blocked_artists.insert(a);
}

void export_snapshot(const Playlist& playlist, PlaybackHistory& history,
                     SongRatingBST& ratings) {
    cout << "--- System Snapshot ---\n";
    auto songs = playlist.all_songs();
    sort(songs.begin(), songs.end(),
         [](auto a, auto b){ return a->duration > b->duration; });
    cout << "Top 5 Longest Songs:\n";
    for (size_t i = 0; i < songs.size() && i < 5; i++)
        cout << songs[i]->display() << "\n";

    cout << "Recently Played:\n";
    for (auto& s : history.recent(5))
        cout << s->display() << "\n";

    auto counts = ratings.rating_counts();
    cout << "Song Count by Rating:\n";
    for (auto& kv : counts)
        cout << kv.first << " stars: " << kv.second << "\n";
}

void suggest_time_fitting_songs(const Playlist& playlist, int available_time_sec) {
    auto songs = playlist.all_songs();
    sort(songs.begin(), songs.end(), [](auto a, auto b) {
        return a->duration < b->duration;  // Greedy: shortest first
    });

    cout << "\n[Suggestion] Songs that fit in " << available_time_sec << " seconds:\n";
    int total = 0;
    bool found = false;
    for (auto& s : songs) {
        if (total + s->duration <= available_time_sec) {
            cout << "- " << s->display() << endl;
            total += s->duration;
            found = true;
        }
    }
    if (!found) cout << "[INFO] No songs fit within given duration.\n";
    else cout << "[Total Duration] " << total << " seconds used.\n";
}

// ================= Main Program =================
int main() {
    Playlist playlist;
    SongLookup lookup;
    FavoriteQueue favorites;
    SongRatingBST rating_tree;
    PlaybackHistory history;

    int song_counter = 1;
    string input;

    while (true) {
        cout << "\n===========================================\n";
        cout << "          PLAYWISE MUSIC SYSTEM\n";
        cout << "===========================================\n";
        cout << " 1.  Add Song\n";
        cout << " 2.  Show Playlist\n";
        cout << " 3.  Delete Song by Index\n";
        cout << " 4.  Move Song\n";
        cout << " 5.  Reverse Playlist\n";
        cout << " 6.  Play Song\n";
        cout << " 7.  Undo Last Play\n";
        cout << " 8.  Rate Song\n";
        cout << " 9.  Lookup Song by Title\n";
        cout << "10.  Show Top Favorites\n";
        cout << "11.  Block Artist\n";
        cout << "12.  Duration Summary\n";
        cout << "13.  Sort Playlist\n";
        cout << "14.  Snapshot\n";
        cout << "15.  Partial Search by Title\n";
        cout << "16.  Suggest Songs by Time\n";
        cout << "17.  Exit\n";
        cout << "===========================================\n";
        cout << "Choose an option: ";
        getline(cin, input);

        if (input == "1") {
            string title, artist, dstr;
            int duration;
            cout << "Enter song title: "; getline(cin, title);
            cout << "Enter artist name: "; getline(cin, artist);
            if (is_blocked_artist(artist)) {
                cout << "[ERROR] Artist is blocked.\n";
                continue;
            }
            if (playlist.exists(title, artist)) {
                cout << "[ERROR] Duplicate song.\n";
                continue;
            }
            cout << "Enter duration (sec): "; getline(cin, dstr);
            if (!all_of(dstr.begin(), dstr.end(), ::isdigit) || stoi(dstr) <= 0) {
                cout << "[ERROR] Invalid duration.\n";
                continue;
            }
            duration = stoi(dstr);
            auto song = make_shared<Song>(song_counter, title, artist, duration);
            playlist.add_song(song); lookup.add_song(song);
            song_counter++;
            cout << "[INFO] Song added.\n";
        }
        else if (input == "2") playlist.show();
        else if (input == "3") {
            string idxstr; int idx;
            cout << "Enter index to delete: "; getline(cin, idxstr);
            if (!all_of(idxstr.begin(), idxstr.end(), ::isdigit)) {
                cout << "[ERROR] Invalid input.\n";
                continue;
            }
            idx = stoi(idxstr);
            playlist.delete_song(idx, [&](shared_ptr<Song> s){ lookup.remove_song(s); rating_tree.delete_song(s->song_id); });
        }
        else if (input == "4") {
            string s1, s2; int from_idx, to_idx;
            cout << "From index: "; getline(cin, s1);
            cout << "To index: "; getline(cin, s2);
            if (!all_of(s1.begin(), s1.end(), ::isdigit) || !all_of(s2.begin(), s2.end(), ::isdigit)) {
                cout << "[ERROR] Invalid input.\n";
                continue;
            }
            from_idx = stoi(s1); to_idx = stoi(s2);
            playlist.move_song(from_idx, to_idx);
        }
        else if (input == "5") playlist.reverse_playlist();
        else if (input == "6") {
            string title;
            cout << "Enter song title: "; getline(cin, title);
            auto song = lookup.get_by_title(title);
            if (song) {
                if (is_blocked_artist(song->artist)) {
                    cout << "[ERROR] Artist is blocked.\n";
                    continue;
                }
                history.play(song);
                song->total_listen_time += song->duration;
                favorites.add_or_update(song);
                cout << "[PLAYING] " << song->display() << endl;
            } else cout << "[ERROR] Song not found.\n";
        }
        else if (input == "7") {
            auto song = history.undo_last_play();
            if (song) {
                if (!playlist.exists(song->title, song->artist)) {
                    playlist.add_song(song);
                    cout << "[INFO] Re-added: " << song->display() << endl;
                } else cout << "[WARN] Song already exists.\n";
            } else cout << "[WARN] No history.\n";
        }
        else if (input == "8") {
            string title, rstr; int rating;
            cout << "Enter song title: "; getline(cin, title);
            cout << "Enter rating (1-5): "; getline(cin, rstr);
            if (!all_of(rstr.begin(), rstr.end(), ::isdigit)) {
                cout << "[ERROR] Invalid rating.\n";
                continue;
            }
            rating = stoi(rstr);
            if (rating < 1 || rating > 5) {
                cout << "[ERROR] Rating must be 1–5.\n";
                continue;
            }
            auto song = lookup.get_by_title(title);
            if (song) {
                rating_tree.insert_or_update(song, rating);
                cout << "[INFO] Rating updated.\n";
            } else cout << "[ERROR] Song not found.\n";
        }
        else if (input == "9") {
            string title;
            cout << "Enter title: "; getline(cin, title);
            auto song = lookup.get_by_title(title);
            cout << (song ? "[FOUND] " + song->display() : "[NOT FOUND]") << endl;
        }
        else if (input == "10") {
            for (auto& s : favorites.get_top_favorites())
                cout << s->title << " - " << s->total_listen_time << " sec\n";
        }
        else if (input == "11") {
            string artist;
            cout << "Enter artist: "; getline(cin, artist);
            block_artist(artist);
            cout << "[INFO] Artist blocked.\n";
        }
        else if (input == "12") playlist_duration_summary(playlist);
        else if (input == "13") {
            cout << "Sort by (1=Title, 2=Duration, 3=Recently Added): ";
            string choice; getline(cin, choice);
            auto songs = playlist.all_songs();
            if (choice == "1")
                songs = quick_sort_songs(songs, [](auto a, auto b){ return a->title < b->title; });
            else if (choice == "2")
                songs = quick_sort_songs(songs, [](auto a, auto b){ return a->duration < b->duration; });
            else if (choice == "3")
                songs = quick_sort_songs(songs, [](auto a, auto b){ return a->song_id > b->song_id; });
            for (auto& s : songs) cout << s->display() << "\n";
        }
        else if (input == "14") export_snapshot(playlist, history, rating_tree);
        else if (input == "15") {
            string term;
            cout << "Enter partial title: "; getline(cin, term);
            auto results = lookup.search_by_partial_title(term);
            if (results.empty()) cout << "[NOT FOUND]\n";
            else for (auto& s : results) cout << s->display() << "\n";
        }
        else if (input == "16") {
            string tstr;
            cout << "Enter available time in seconds: ";
            getline(cin, tstr);
            if (!all_of(tstr.begin(), tstr.end(), ::isdigit)) {
                cout << "[ERROR] Invalid input.\n";
                continue;
            }
            int time_limit = stoi(tstr);
            suggest_time_fitting_songs(playlist, time_limit);
        }
        else if (input == "17") break;
        else cout << "[ERROR] Invalid choice.\n";
    }
    return 0;
}