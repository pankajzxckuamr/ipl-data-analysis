#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <map>
#include <cmath>
#include <numeric>
#include <limits>

using namespace std;

// Define a struct to store match data
struct Match
{
    string season;
    string match_name;
    string fullName;
    int runs;
    float strikeRate;
    int sixes;
    int fours;
};

// Forward declarations of all functions
string toLowercase(const string &str);
vector<string> splitLine(const string &line);
vector<Match> loadMatchData(const string &filename);
vector<Match> filterMatches(const vector<Match> &matches, const string &filterValue, const string &filterType);
void writeMatchDataToFile(const vector<Match> &matches, const string &filterValue, const string &filterType);
void sortMatches(vector<Match> &matches, const string &sortType);
void findTopPlayer(const vector<Match> &matches, const string &criteria);
vector<Match> filterByRuns(const vector<Match> &matches, int minRuns);
void sortMatchesBySixesOrFours(vector<Match> &matches, const string &criteria);
void compareTeamPerformance(const vector<Match> &matches);
void extractHighScoringMatches(const vector<Match> &matches, int minRuns);
void analyzePlayerConsistency(const vector<Match> &matches, const string &playerName);
void analyzePlayerSessionConsistency(const vector<Match> &matches, const string &playerName, const string &season);
void predictPerformance(const vector<Match> &matches, const string &playerName);
void comparePlayersHeadToHead(const vector<Match> &matches, const string &player1, const string &player2);
void createBarChart(const vector<Match> &matches, const string &title);
void createHistogram(const vector<Match> &matches, int binCount = 10);
void showMenu();
void executeOperation(vector<Match> &matches);
void handleBasicOperations(vector<Match> &matches);
void handleStatisticalAnalysis(vector<Match> &matches);
void handlePlayerAnalysis(vector<Match> &matches);
void handlePredictionsAndVisualizations(vector<Match> &matches);

// Convert a string to lowercase
string toLowercase(const string &str)
{
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Function to split a CSV line into tokens
vector<string> splitLine(const string &line)
{
    vector<string> values;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ','))
    {
        values.push_back(token);
    }
    return values;
}

// Function to load match data from CSV file
vector<Match> loadMatchData(const string &filename)
{
    ifstream file(filename);
    vector<Match> matches;

    if (!file)
    {
        cout << "Error: File not found! Check the path and filename.\n";
        return matches;
    }

    cout << "File opened successfully!\n";

    string line;
    getline(file, line); // Skip header row

    while (getline(file, line))
    {
        vector<string> values = splitLine(line);
        if (values.size() >= 5)
        {
            try
            {
                Match match;
                match.season = values[0];
                match.match_name = values[1];
                match.fullName = values[2];
                match.runs = stoi(values[3]);
                match.strikeRate = stof(values[4]);

                // Handle optional fields with defaults
                match.sixes = (values.size() > 5) ? stoi(values[5]) : 0;
                match.fours = (values.size() > 6) ? stoi(values[6]) : 0;

                matches.push_back(match);
            }
            catch (const exception &e)
            {
                cout << "Skipping invalid row: " << line << "\n";
                continue;
            }
        }
        else
        {
            cout << "Skipping row with insufficient columns: " << line << "\n";
        }
    }

    file.close();
    cout << "Data loaded successfully! Total matches: " << matches.size() << "\n";

    return matches;
}

// Function to filter matches based on player name, team, or season
vector<Match> filterMatches(const vector<Match> &matches, const string &filterValue, const string &filterType)
{
    vector<Match> filteredMatches;
    string lowerFilterValue = toLowercase(filterValue);

    for (const auto &match : matches)
    {
        if ((filterType == "player" && toLowercase(match.fullName) == lowerFilterValue) ||
            (filterType == "team" && toLowercase(match.match_name).find(lowerFilterValue) != string::npos) ||
            (filterType == "season" && match.season == filterValue))
        {
            filteredMatches.push_back(match);
        }
    }
    return filteredMatches;
}

// Function to write match data to a file
void writeMatchDataToFile(const vector<Match> &matches, const string &filterValue, const string &filterType)
{
    string filename = filterValue + "_stats.txt";
    ofstream outFile(filename);
    if (!outFile)
    {
        cout << "Error creating output file!\n";
        return;
    }

    outFile << "Statistics for " << filterValue << " (" << filterType << ")\n";
    outFile << string(50, '=') << "\n\n";

    for (const auto &match : matches)
    {
        outFile << "Season: " << match.season << "\n";
        outFile << "Match:  " << match.match_name << "\n";
        outFile << "Runs: " << match.runs << "\n";
        outFile << "Strike Rate: " << match.strikeRate << "\n";
        outFile << "Sixes: " << match.sixes << "\n";
        outFile << "Fours: " << match.fours << "\n\n";
    }

    cout << "Statistics saved to " << filename << "\n";
}

// Function to sort matches based on runs, strike rate, or sixes
void sortMatches(vector<Match> &matches, const string &sortType)
{
    if (sortType == "runs")
    {
        sort(matches.begin(), matches.end(), [](const Match &a, const Match &b)
             { return a.runs > b.runs; });
    }
    else if (sortType == "strikeRate")
    {
        sort(matches.begin(), matches.end(), [](const Match &a, const Match &b)
             { return a.strikeRate > b.strikeRate; });
    }
    else if (sortType == "sixes")
    {
        sort(matches.begin(), matches.end(), [](const Match &a, const Match &b)
             { return a.sixes > b.sixes; });
    }
    else
    {
        cout << "Invalid sort type! Use 'runs', 'strikeRate', or 'sixes'.\n";
        return;
    }

    cout << "\nMatches sorted by " << sortType << ":\n";
    cout << left << setw(10) << "Season" << setw(30) << "Match"
         << setw(25) << "Player" << setw(10) << sortType << "\n";
    cout << string(75, '-') << "\n";

    for (size_t i = 0; i < min(size_t(10), matches.size()); i++)
    {
        cout << left << setw(10) << matches[i].season
             << setw(30) << matches[i].match_name
             << setw(25) << matches[i].fullName;

        if (sortType == "runs")
        {
            cout << setw(10) << matches[i].runs << "\n";
        }
        else if (sortType == "strikeRate")
        {
            cout << setw(10) << fixed << setprecision(2) << matches[i].strikeRate << "\n";
        }
        else if (sortType == "sixes")
        {
            cout << setw(10) << matches[i].sixes << "\n";
        }
    }
}

// Function to find the player with the highest sixes, fours, or runs
void findTopPlayer(const vector<Match> &matches, const string &criteria)
{
    if (criteria != "sixes" && criteria != "fours" && criteria != "runs")
    {
        cout << "Invalid criteria! Use 'sixes', 'fours', or 'runs'.\n";
        return;
    }

    auto topPlayer = max_element(matches.begin(), matches.end(), [&criteria](const Match &a, const Match &b)
                                 {
        if (criteria == "sixes")
            return a.sixes < b.sixes;
        else if (criteria == "fours")
            return a.fours < b.fours;
        else
            return a.runs < b.runs; });

    if (topPlayer != matches.end())
    {
        cout << "Top player based on " << criteria << ":\n";
        cout << "Player: " << topPlayer->fullName << "\n";
        cout << "Match: " << topPlayer->match_name << "\n";
        cout << "Season: " << topPlayer->season << "\n";
        cout << criteria << ": " << (criteria == "sixes" ? topPlayer->sixes : (criteria == "fours" ? topPlayer->fours : topPlayer->runs)) << "\n";
    }
    else
    {
        cout << "No data available to find the top player.\n";
    }
}

// Function to filter players or matches with runs greater than a specific value
vector<Match> filterByRuns(const vector<Match> &matches, int minRuns)
{
    vector<Match> filteredMatches;
    for (const auto &match : matches)
    {
        if (match.runs > minRuns)
        {
            filteredMatches.push_back(match);
        }
    }
    return filteredMatches;
}

// Function to sort matches based on sixes or fours
void sortMatchesBySixesOrFours(vector<Match> &matches, const string &criteria)
{
    if (criteria != "sixes" && criteria != "fours")
    {
        cout << "Invalid criteria! Use 'sixes' or 'fours'.\n";
        return;
    }

    sort(matches.begin(), matches.end(), [&criteria](const Match &a, const Match &b)
         {
        if (criteria == "sixes")
            return a.sixes > b.sixes;
        else
            return a.fours > b.fours; });

    cout << "Matches sorted by " << criteria << ":\n";
    for (size_t i = 0; i < min(size_t(10), matches.size()); i++)
    {
        cout << "Season: " << matches[i].season << " | Match: " << matches[i].match_name
             << " | " << criteria << ": " << (criteria == "sixes" ? matches[i].sixes : matches[i].fours) << "\n";
    }
}

// Function to compare batting performance of different teams
void compareTeamPerformance(const vector<Match> &matches)
{
    // Map to store team performance data
    map<string, pair<int, int>> teamPerformance; // {team_name: {total_runs, match_count}}

    for (const auto &match : matches)
    {
        teamPerformance[match.match_name].first += match.runs;
        teamPerformance[match.match_name].second++;
    }

    cout << "\nTeam Performance Comparison:\n";
    cout << left << setw(30) << "Team Name" << setw(15) << "Total Runs"
         << setw(15) << "Matches Played" << setw(15) << "Average Runs" << "\n";
    cout << string(75, '-') << "\n";

    for (const auto &entry : teamPerformance)
    {
        const string &teamName = entry.first;
        int totalRuns = entry.second.first;
        int matchesPlayed = entry.second.second;
        float averageRuns = matchesPlayed > 0 ? static_cast<float>(totalRuns) / matchesPlayed : 0.0f;

        cout << left << setw(30) << teamName << setw(15) << totalRuns
             << setw(15) << matchesPlayed << setw(15) << fixed << setprecision(2) << averageRuns << "\n";
    }
}

// Function to extract details about high-scoring matches
void extractHighScoringMatches(const vector<Match> &matches, int minRuns)
{
    vector<Match> highScoreMatches;

    // Filter matches above minimum runs
    for (const auto &match : matches)
    {
        if (match.runs > minRuns)
        {
            highScoreMatches.push_back(match);
        }
    }

    // Sort by runs in descending order
    sort(highScoreMatches.begin(), highScoreMatches.end(),
         [](const Match &a, const Match &b)
         { return a.runs > b.runs; });

    cout << "\nHigh-Scoring Matches (Runs > " << minRuns << "):\n";
    cout << "Total matches found: " << highScoreMatches.size() << "\n";
    cout << left << setw(15) << "Season" << setw(30) << "Match Name"
         << setw(25) << "Player Name" << setw(10) << "Runs"
         << setw(15) << "Strike Rate" << "\n";
    cout << string(95, '-') << "\n";

    for (const auto &match : highScoreMatches)
    {
        cout << left << setw(15) << match.season
             << setw(30) << match.match_name
             << setw(25) << match.fullName
             << setw(10) << match.runs
             << setw(15) << fixed << setprecision(2) << match.strikeRate << "\n";
    }
}

// Function to analyze player consistency across seasons
void analyzePlayerConsistency(const vector<Match> &matches, const string &playerName)
{
    map<string, vector<int>> seasonRuns; // {season: [runs]}

    for (const auto &match : matches)
    {
        if (toLowercase(match.fullName) == toLowercase(playerName))
        {
            seasonRuns[match.season].push_back(match.runs);
        }
    }

    if (seasonRuns.empty())
    {
        cout << "No data found for player: " << playerName << "\n";
        return;
    }

    cout << "\nConsistency Analysis for " << playerName << ":\n";
    cout << left << setw(10) << "Season" << setw(15) << "Total Runs"
         << setw(15) << "Avg Runs" << setw(15) << "Max Runs" << "\n";
    cout << string(55, '-') << "\n";

    for (const auto &season : seasonRuns)
    {
        int totalRuns = 0;
        int maxRuns = 0;
        for (int runs : season.second)
        {
            totalRuns += runs;
            maxRuns = max(maxRuns, runs);
        }
        float avgRuns = static_cast<float>(totalRuns) / season.second.size();

        cout << left << setw(10) << season.first
             << setw(15) << totalRuns
             << setw(15) << fixed << setprecision(2) << avgRuns
             << setw(15) << maxRuns << "\n";
    }
}

// Function to analyze player consistency across matches in a session
void analyzePlayerSessionConsistency(const vector<Match> &matches, const string &playerName, const string &season)
{
    vector<int> matchRuns;

    for (const auto &match : matches)
    {
        if (toLowercase(match.fullName) == toLowercase(playerName) && match.season == season)
        {
            matchRuns.push_back(match.runs);
        }
    }

    if (matchRuns.empty())
    {
        cout << "No matches found for " << playerName << " in season " << season << "\n";
        return;
    }

    double sum = 0, variance = 0;
    int maxRuns = 0, minRuns = INT_MAX;

    for (int runs : matchRuns)
    {
        sum += runs;
        maxRuns = max(maxRuns, runs);
        minRuns = min(minRuns, runs);
    }

    double mean = sum / matchRuns.size();

    for (int runs : matchRuns)
    {
        variance += pow(runs - mean, 2);
    }
    variance /= matchRuns.size();

    cout << "\nSession Consistency Analysis for " << playerName << " (Season " << season << "):\n";
    cout << "Matches played: " << matchRuns.size() << "\n";
    cout << "Average runs: " << fixed << setprecision(2) << mean << "\n";
    cout << "Standard deviation: " << fixed << setprecision(2) << sqrt(variance) << "\n";
    cout << "Highest score: " << maxRuns << "\n";
    cout << "Lowest score: " << minRuns << "\n";
}

// Function to perform linear regression and predict future performance
void predictPerformance(const vector<Match> &matches, const string &playerName)
{
    vector<pair<int, int>> data; // {match_number, runs}
    int matchNum = 1;

    for (const auto &match : matches)
    {
        if (toLowercase(match.fullName) == toLowercase(playerName))
        {
            data.push_back({matchNum++, match.runs});
        }
    }

    if (data.size() < 2)
    {
        cout << "Insufficient data for prediction\n";
        return;
    }

    // Calculate means
    double sumX = 0, sumY = 0;
    for (const auto &point : data)
    {
        sumX += point.first;
        sumY += point.second;
    }
    double meanX = sumX / data.size();
    double meanY = sumY / data.size();

    // Calculate coefficients
    double numerator = 0, denominator = 0;
    for (const auto &point : data)
    {
        numerator += (point.first - meanX) * (point.second - meanY);
        denominator += (point.first - meanX) * (point.first - meanX);
    }

    double slope = denominator != 0 ? numerator / denominator : 0;
    double intercept = meanY - slope * meanX;

    // Predict next 3 matches
    cout << "\nPerformance Prediction for " << playerName << ":\n";
    cout << "Based on " << data.size() << " matches\n";
    cout << "Trend: " << (slope > 0 ? "Improving" : "Declining") << "\n\n";

    for (int i = 1; i <= 3; i++)
    {
        int nextMatch = data.size() + i;
        int predictedRuns = round(slope * nextMatch + intercept);
        predictedRuns = max(0, predictedRuns); // Ensure non-negative runs
        cout << "Predicted runs for next match " << i << ": " << predictedRuns << "\n";
    }
}

// Function to compare two players head-to-head
void comparePlayersHeadToHead(const vector<Match> &matches, const string &player1, const string &player2)
{
    map<string, vector<int>> player1Stats, player2Stats;

    // Collect stats for both players
    for (const auto &match : matches)
    {
        if (toLowercase(match.fullName) == toLowercase(player1))
        {
            player1Stats[match.season].push_back(match.runs);
        }
        else if (toLowercase(match.fullName) == toLowercase(player2))
        {
            player2Stats[match.season].push_back(match.runs);
        }
    }

    if (player1Stats.empty() || player2Stats.empty())
    {
        cout << "Insufficient data for comparison\n";
        return;
    }

    cout << "\nHead-to-Head Comparison: " << player1 << " vs " << player2 << "\n";
    cout << string(60, '-') << "\n";
    cout << left << setw(10) << "Season"
         << setw(15) << player1 + " Avg"
         << setw(15) << player2 + " Avg"
         << setw(20) << "Matches Played\n";
    cout << string(60, '-') << "\n";

    // Compare season by season
    for (const auto &season : player1Stats)
    {
        if (player2Stats.find(season.first) != player2Stats.end())
        {
            double avg1 = accumulate(season.second.begin(), season.second.end(), 0.0) / season.second.size();
            double avg2 = accumulate(player2Stats[season.first].begin(), player2Stats[season.first].end(), 0.0) / player2Stats[season.first].size();

            cout << left << setw(10) << season.first
                 << setw(15) << fixed << setprecision(2) << avg1
                 << setw(15) << avg2
                 << setw(10) << min(season.second.size(), player2Stats[season.first].size()) << "\n";
        }
    }
}

// Function to create a bar chart visualization
void createBarChart(const vector<Match> &matches, const string &title)
{
    map<string, int> data;
    const int WIDTH = 50; // Maximum width of the bar

    // Aggregate data
    for (const auto &match : matches)
    {
        data[match.season] += match.runs;
    }

    // Find maximum value for scaling
    int maxValue = 0;
    for (const auto &pair : data)
    {
        maxValue = max(maxValue, pair.second);
    }

    // Print bar chart
    cout << "\n"
         << title << "\n";
    cout << string(60, '=') << "\n";

    for (const auto &pair : data)
    {
        int barLength = (pair.second * WIDTH) / maxValue;
        cout << left << setw(10) << pair.first << " |";
        cout << string(barLength, '*') << " " << pair.second << "\n";
    }
}

// Function to create a simple histogram
void createHistogram(const vector<Match> &matches, int binCount)
{
    vector<int> bins(binCount, 0);
    int maxRuns = 0;

    // Find maximum runs for bin sizing
    for (const auto &match : matches)
    {
        maxRuns = max(maxRuns, match.runs);
    }

    // Calculate bin size and populate bins
    int binSize = (maxRuns + binCount - 1) / binCount;
    for (const auto &match : matches)
    {
        int binIndex = min(match.runs / binSize, binCount - 1);
        bins[binIndex]++;
    }

    // Print histogram
    cout << "\nRuns Distribution Histogram\n";
    cout << string(60, '=') << "\n";

    for (int i = 0; i < binCount; i++)
    {
        cout << setw(3) << i * binSize << "-" << setw(3) << (i + 1) * binSize - 1 << " |";
        cout << string(bins[i], '*') << " " << bins[i] << "\n";
    }
}

// Menu function to show available operations
void showMenu()
{
    cout << "\nBatting Data Analysis System\n";
    cout << "============================\n";
    cout << "1. Basic Operations\n";
    cout << "   - Filter and display data\n";
    cout << "   - Sort matches by runs/strike rate\n";
    cout << "2. Statistical Analysis\n";
    cout << "   - Find top performers\n";
    cout << "   - Compare team performance\n";
    cout << "   - Analyze high-scoring matches\n";
    cout << "3. Player Analysis\n";
    cout << "   - Analyze player consistency\n";
    cout << "   - Compare players head-to-head\n";
    cout << "4. Predictions & Visualizations\n";
    cout << "   - Predict player performance\n";
    cout << "   - View statistical charts\n";
    cout << "0. Exit\n";
}

// Handler for basic operations menu option
void handleBasicOperations(vector<Match> &matches)
{
    cout << "\nBasic Operations:\n";
    cout << "1. Filter data\n2. Sort matches\n";
    int subChoice;
    cin >> subChoice;

    if (subChoice == 1)
    {
        cout << "Filter by: 1-Player, 2-Team, 3-Season\n";
        int filterChoice;
        cin >> filterChoice;
        cin.ignore();

        string value;
        cout << "Enter value: ";
        getline(cin, value);

        string type = (filterChoice == 1) ? "player" : (filterChoice == 2) ? "team"
                                                                           : "season";

        auto filtered = filterMatches(matches, value, type);

        if (filtered.empty())
        {
            cout << "No matches found for " << value << " in " << type << " category.\n";
        }
        else
        {
            cout << "\nFound " << filtered.size() << " matches for " << value << " (" << type << "):\n";
            for (const auto &match : filtered)
            {
                cout << match.season << " | " << match.match_name << " | " << match.fullName
                     << " | Runs: " << match.runs << " | Strike Rate: " << match.strikeRate << "\n";
            }
            writeMatchDataToFile(filtered, value, type);
        }
    }
    else if (subChoice == 2)
    {
        cout << "Sort by: 1-Runs, 2-Strike Rate, 3-Sixes\n";
        int sortChoice;
        cin >> sortChoice;

        if (sortChoice == 1)
        {
            sortMatches(matches, "runs");
        }
        else if (sortChoice == 2)
        {
            sortMatches(matches, "strikeRate");
        }
        else if (sortChoice == 3)
        {
            sortMatches(matches, "sixes");
        }
        else
        {
            cout << "Invalid choice!\n";
        }
    }
    else
    {
        cout << "Invalid choice!\n";
    }
}

// Handler for statistical analysis menu option
void handleStatisticalAnalysis(vector<Match> &matches)
{
    cout << "\nStatistical Analysis:\n";
    cout << "1. Top performers\n2. Team comparison\n3. High-scoring matches\n";
    int subChoice;
    cin >> subChoice;

    if (subChoice == 1)
    {
        cout << "Find top player based on: 1-Runs, 2-Sixes, 3-Fours\n";
        int criteriaChoice;
        cin >> criteriaChoice;

        string criteria;
        if (criteriaChoice == 1)
            criteria = "runs";
        else if (criteriaChoice == 2)
            criteria = "sixes";
        else if (criteriaChoice == 3)
            criteria = "fours";
        else
        {
            cout << "Invalid choice!\n";
            return;
        }

        findTopPlayer(matches, criteria);
    }
    else if (subChoice == 2)
    {
        compareTeamPerformance(matches);
    }
    else if (subChoice == 3)
    {
        cout << "Enter minimum runs: ";
        int minRuns;
        cin >> minRuns;
        extractHighScoringMatches(matches, minRuns);
    }
    else
    {
        cout << "Invalid choice!\n";
    }
}

// Handler for player analysis menu option
void handlePlayerAnalysis(vector<Match> &matches)
{
    cout << "\nPlayer Analysis:\n";
    cout << "1. Player consistency\n2. Head-to-head comparison\n3. Session consistency\n";
    int subChoice;
    cin >> subChoice;
    cin.ignore();

    if (subChoice == 1)
    {
        string player;
        cout << "Enter player name: ";
        getline(cin, player);
        analyzePlayerConsistency(matches, player);
    }
    else if (subChoice == 2)
    {
        string player1, player2;
        cout << "Enter first player: ";
        getline(cin, player1);
        cout << "Enter second player: ";
        getline(cin, player2);
        comparePlayersHeadToHead(matches, player1, player2);
    }
    else if (subChoice == 3)
    {
        string player, season;
        cout << "Enter player name: ";
        getline(cin, player);
        cout << "Enter season (e.g., 2020): ";
        getline(cin, season);
        analyzePlayerSessionConsistency(matches, player, season);
    }
    else
    {
        cout << "Invalid choice!\n";
    }
}

// Handler for predictions and visualizations menu option
void handlePredictionsAndVisualizations(vector<Match> &matches)
{
    cout << "\nPredictions & Visualizations:\n";
    cout << "1. Predict performance\n2. View bar chart\n3. View histogram\n";
    int subChoice;
    cin >> subChoice;
    cin.ignore();

    if (subChoice == 1)
    {
        string player;
        cout << "Enter player name: ";
        getline(cin, player);
        predictPerformance(matches, player);
    }
    else if (subChoice == 2)
    {
        createBarChart(matches, "Runs by Season");
    }
    else if (subChoice == 3)
    {
        int bins;
        cout << "Enter number of bins for histogram (5-20): ";
        cin >> bins;
        bins = max(5, min(20, bins)); // Ensure reasonable bin count
        createHistogram(matches, bins);
    }
    else
    {
        cout << "Invalid choice!\n";
    }
}

// Function to handle user input and execute operations
void executeOperation(vector<Match> &matches)
{
    int choice;
    do
    {
        showMenu();
        cout << "\nEnter your choice (0-4): ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            handleBasicOperations(matches);
            break;
        case 2:
            handleStatisticalAnalysis(matches);
            break;
        case 3:
            handlePlayerAnalysis(matches);
            break;
        case 4:
            handlePredictionsAndVisualizations(matches);
            break;
        case 0:
            cout << "Exiting program...\n";
            break;
        default:
            cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);
}

int main()
{
    string filename;
    cout << "Enter path to CSV file (e.g., C:\\data\\batting_card.csv): ";
    getline(cin, filename);

    vector<Match> matches = loadMatchData(filename);
    if (!matches.empty())
    {
        executeOperation(matches);
    }
    else
    {
        cout << "No data loaded. Please check the file and try again.\n";
    }

    return 0;
}

// batting: C:\\Users\\panka\\Desktop\\sem4\\project\\all_season_batting_card.csv
// bowling: C:\\Users\\panka\\Desktop\\sem4\\project\\all_season_bowling_card.csv
// all_season_batting_card.csv: C:\\Users\\panka\\Desktop\\sem4\\project\\all_season_batting_card.csv
// all_season_bowling_card.csv: C:\\Users\\panka\\Desktop\\sem4\\project\\all_season_bowling_card.csv
