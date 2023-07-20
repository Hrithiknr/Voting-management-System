#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

const string CANDIDATE_FILE = "candidates.txt";
const string VOTE_FILE = "votes.txt";
const string PARTY_VOTES_FILE = "party_votes.txt";
const string VOTER_FILE = "voters.txt";

struct Candidate {
    string name;
    string party;
    string symbol;
    int votes;
};

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<Candidate> loadCandidates() {
    vector<Candidate> candidates;
    ifstream candidateFile(CANDIDATE_FILE);
    string line;
    while (getline(candidateFile, line)) {
        vector<string> candidateData = split(line, '|');
        Candidate candidate;
        candidate.name = candidateData[0];
        candidate.party = candidateData[1];
        candidate.symbol = candidateData[2];
        candidate.votes = stoi(candidateData[3]);
        candidates.push_back(candidate);
    }
    candidateFile.close();
    return candidates;
}

void saveCandidates(const vector<Candidate>& candidates) {
    ofstream candidateFile(CANDIDATE_FILE);
    for (const Candidate& candidate : candidates) {
        candidateFile << candidate.name << "|" << candidate.party << "|" << candidate.symbol << "|" << candidate.votes << endl;
    }
    candidateFile.close();
}

void seeResult() {
    cout << "Voting Result" << endl;
    cout << "------------------------" << endl;

    vector<Candidate> candidates = loadCandidates();
    ifstream partyVotesFile(PARTY_VOTES_FILE);
    string line;

    while (getline(partyVotesFile, line)) {
        vector<string> partyVotesData = split(line, '|');
        string party = partyVotesData[0];
        int votes = stoi(partyVotesData[1]);
        cout << party << ": " << votes << " vote(s)" << endl;
    }
    partyVotesFile.close();

    cout << "------------------------" << endl;
    cout << "Candidates" << endl;
    cout << "------------------------" << endl;
    for (const Candidate& candidate : candidates) {
        cout << candidate.name << " (" << candidate.party << "): " << candidate.votes << " vote(s)" << endl;
    }
    cout << "------------------------" << endl;
}

void addCandidate() {
    cout << "Adding New Candidate" << endl;
    cout << "------------------------" << endl;

    Candidate candidate;
    cout << "Enter Candidate Name: ";
    cin.ignore();
    getline(cin, candidate.name);
    cout << "Enter Party Name: ";
    getline(cin, candidate.party);
    cout << "Enter Party Symbol: ";
    getline(cin, candidate.symbol);

    candidate.votes = 0;

    vector<Candidate> candidates = loadCandidates();
    candidates.push_back(candidate);
    saveCandidates(candidates);

    cout << "New candidate added successfully!" << endl;
}

void deleteVoter() {
    cout << "Delete Voter" << endl;
    cout << "------------------------" << endl;

    string voterID;
    cout << "Enter Voter ID: ";
    cin >> voterID;

    ifstream voterFile(VOTER_FILE);
    ofstream tempFile("temp.txt");

    string line;
    bool isVoterFound = false;

    while (getline(voterFile, line)) {
        if (line.find(voterID) != string::npos) {
            isVoterFound = true;
        } else {
            tempFile << line << endl;
        }
    }

    voterFile.close();
    tempFile.close();

    if (isVoterFound) {
        remove(VOTER_FILE.c_str());
        rename("temp.txt", VOTER_FILE.c_str());
        cout << "Voter deleted successfully!" << endl;
    } else {
        cout << "Voter not found!" << endl;
        remove("temp.txt");
    }
}

void deleteAllVoters() {
    cout << "Delete All Voters" << endl;
    cout << "------------------------" << endl;

    cout << "Are you sure you want to delete all voters? This action cannot be undone. (y/n): ";
    char choice;
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        ofstream voterFile(VOTER_FILE);
        voterFile.close();
        cout << "All voters deleted successfully!" << endl;
    } else {
        cout << "Delete all voters operation cancelled!" << endl;
    }
}

void reset() {
    cout << "Reset Voting Data" << endl;
    cout << "------------------------" << endl;

    cout << "Are you sure you want to reset all voting data? This action cannot be undone. (y/n): ";
    char choice;
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        ofstream candidateFile(CANDIDATE_FILE);
        candidateFile.close();
        ofstream voteFile(VOTE_FILE);
        voteFile.close();
        ofstream partyVotesFile(PARTY_VOTES_FILE);
        partyVotesFile.close();
        cout << "Voting data reset successfully!" << endl;
    } else {
        cout << "Reset voting data operation cancelled!" << endl;
    }
}

void addVoters() {
    cout << "Add New Voters" << endl;
    cout << "------------------------" << endl;

    int count;
    cout << "Enter the number of voters to add: ";
    cin >> count;

    ofstream voterFile(VOTER_FILE, ofstream::app);

    for (int i = 0; i < count; i++) {
        string voterID;
        string birthdate;
        string name;

        cout << "Enter the Name of the Voter " << (i + 1) << ": ";
        cin >> name;
        cout << "Enter Voter ID of " <<name<< ": ";
        cin >> voterID;

        ifstream voterCheckFile(VOTER_FILE);
        string line;
        bool isVoterExist = false;

        while (getline(voterCheckFile, line)) {
            if (line.find(voterID) != string::npos) {
                isVoterExist = true;
                break;
            }
        }
        voterCheckFile.close();

        if (isVoterExist) {
            cout << "Voter with ID " << voterID << " already exists. Skipping..." << endl;
            continue;
        }

        cout << "Enter Birthdate (dd/mm/yyyy) for Voter " << (i + 1) << ": ";
        cin >> birthdate;

        // Parsing birthdate
        tm bd_tm{};
        istringstream bd_stream(birthdate);
        bd_stream >> get_time(&bd_tm, "%d/%m/%Y");

        if (bd_stream.fail()) {
            cout << "Invalid birthdate format! Skipping voter..." << endl;
            continue;
        }

        // Getting current date
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        tm* current_tm = localtime(&now_time);

        // Comparing birthdate with current date
        if (current_tm->tm_year - bd_tm.tm_year < 18 ||
            (current_tm->tm_year - bd_tm.tm_year == 18 && current_tm->tm_mon < bd_tm.tm_mon) ||
            (current_tm->tm_year - bd_tm.tm_year == 18 && current_tm->tm_mon == bd_tm.tm_mon && current_tm->tm_mday < bd_tm.tm_mday)) {
            cout << "Voter with ID " << voterID << " is not eligible to vote (age < 18). Skipping..." << endl;
            continue;
        }

        voterFile << voterID << "|" <<name<< "|" <<(current_tm->tm_year + 1900 - bd_tm.tm_year)<<" |    ";
        cout << "Voter with ID " << voterID << " added successfully!" << endl;
    }

    voterFile.close();
    cout << "New voters added successfully!" << endl;
}



void giveVote() {
    cout << "Give Vote" << endl;
    cout << "------------------------" << endl;

    string voterID;
    cout << "Enter Voter ID: ";
    cin >> voterID;

    ifstream voterFile(VOTER_FILE);
    string line;
    bool isVoterFound = false;

    while (getline(voterFile, line)) {
        if (line.find(voterID) != string::npos) {
            isVoterFound = true;
            break;
        }
    }

    voterFile.close();

    if (!isVoterFound) {
        cout << "Invalid Voter ID!" << endl;
        return;
    }

    ifstream voteFile(VOTE_FILE);
    bool isVoterVoted = false;

    while (getline(voteFile, line)) {
        if (line.find(voterID) != string::npos) {
            isVoterVoted = true;
            break;
        }
    }

    voteFile.close();

    if (isVoterVoted) {
        cout << "Voter with ID " << voterID << " has already voted!" << endl;
        return;
    }

    vector<Candidate> candidates = loadCandidates();

    cout << "Available Candidates:" << endl;
    for (int i = 0; i < candidates.size(); i++) {
        cout << (i + 1) << ". " << candidates[i].name << " (" << candidates[i].party << ")" << endl;
    }

    int choice;
    cout << "Enter your choice (1-" << candidates.size() << "): ";
    cin >> choice;

    if (choice < 1 || choice > candidates.size()) {
        cout << "Invalid choice!" << endl;
        return;
    }

    int candidateIndex = choice - 1;
    candidates[candidateIndex].votes++;
    saveCandidates(candidates);

    ofstream voteRecordFile(VOTE_FILE, ofstream::app);
    voteRecordFile << voterID << "|" << candidates[candidateIndex].name << "|" << candidates[candidateIndex].party << endl;
    voteRecordFile.close();

    ofstream partyVotesFile(PARTY_VOTES_FILE, ofstream::app);
    partyVotesFile << candidates[candidateIndex].party << "|1" << endl;
    partyVotesFile.close();

    cout << "Voter with ID " << voterID << " has voted successfully for " << candidates[candidateIndex].name << " (" << candidates[candidateIndex].party << ")" << endl;
}

void adminSection() {
    cout << "Admin Section" << endl;
    cout << "------------------------" << endl;

    string adminName;
    cout << "Enter Admin Name: ";
    cin.ignore();
    getline(cin, adminName);

    string password;
    cout << "Enter Password: ";
    getline(cin, password);

    if (adminName != "admin" || password != "admin") {
        cout << "Invalid credentials!" << endl;
        return;
    }

    int choice;
    while (true) {
        cout << endl;
        cout << "Admin Menu" << endl;
        cout << "------------------------" << endl;
        cout << "1. See Result" << endl;
        cout << "2. Enrollment of Candidates" << endl;
        cout << "3. Delete Voter" << endl;
        cout << "4. Delete All Voters" << endl;
        cout << "5. Reset Voting Data" << endl;
        cout << "6. Add New Voters" << endl;
        cout << "7. Exit Admin Section" << endl;
        cout << "------------------------" << endl;
        cout << "Enter your choice (1-7): ";
        cin >> choice;

        switch (choice) {
            case 1:
                seeResult();
                break;
            case 2:
                addCandidate();
                break;
            case 3:
                deleteVoter();
                break;
            case 4:
                deleteAllVoters();
                break;
            case 5:
                reset();
                break;
            case 6:
                addVoters();
                break;
            case 7:
                return;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

void startVoting() {
    cout << "Start Voting" << endl;
    cout << "------------------------" << endl;

    int choice;
    while (true) {
        cout << endl;
        cout << "Start Voting Menu" << endl;
        cout << "------------------------" << endl;
        cout << "1. Give Vote" << endl;
        cout << "2. End Vote" << endl;
        cout << "------------------------" << endl;
        cout << "Enter your choice (1-2): ";
        cin >> choice;

        switch (choice) {
            case 1:
                giveVote();
                break;
            case 2:
                return;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

int main() {
    int choice;
    while (true) {
        cout << "Voting Management System" << endl;
        cout << "------------------------" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. Start Voting" << endl;
        cout << "3. Exit" << endl;
        cout << "------------------------" << endl;
        cout << "Enter your choice (1-3): ";
        cin >> choice;

        switch (choice) {
            case 1:
                adminSection();
                break;
            case 2:
                startVoting();
                break;
            case 3:
                cout << "Exiting the program. Goodbye!" << endl;
                return 0;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}
