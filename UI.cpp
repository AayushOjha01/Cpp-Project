#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>
#include <filesystem> // Include for filesystem operations

enum class Page {
    Home,
    MusicLibrary,
    Playlists
};

// Function to get the base name of a file (removes the path and extension)
std::string getBaseName(const std::string& filePath) {
    std::filesystem::path p(filePath);
    return p.stem().string();  // Get the file name without the extension
}

class AudioPlayer {
public:
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void next() = 0;
    virtual void previous() = 0;
    virtual void loop(bool loop) = 0;
    virtual void shuffle(bool shuffle) = 0;
    virtual void playSong(int index) = 0;
    virtual ~AudioPlayer() {}
};

class MusicPlayer : public AudioPlayer {
public:
    MusicPlayer(const std::vector<std::string>& musicFiles)
        : musicFiles(musicFiles), currentIndex(0), isLooping(false), isShuffled(false) {
        if (!musicFiles.empty()) {
            music.openFromFile(musicFiles[currentIndex]);
        }
        std::srand(std::time(nullptr));  // Seed for shuffling
    }

    void play() override {
        if (music.getStatus() != sf::Music::Playing) {
            music.play();
        }
    }

    void pause() override {
        if (music.getStatus() == sf::Music::Playing) {
            music.pause();
        }
    }

    void stop() override {
        music.stop();
    }

    void next() override {
        if (isShuffled && !shuffleIndices.empty()) {
            currentIndex = (currentIndex + 1) % shuffleIndices.size();
            music.openFromFile(musicFiles[shuffleIndices[currentIndex]]);
        }
        else {
            currentIndex = (currentIndex + 1) % musicFiles.size();
            music.openFromFile(musicFiles[currentIndex]);
        }
        play();
    }

    void previous() override {
        if (isShuffled && !shuffleIndices.empty()) {
            currentIndex = (currentIndex == 0) ? shuffleIndices.size() - 1 : currentIndex - 1;
            music.openFromFile(musicFiles[shuffleIndices[currentIndex]]);
        }
        else {
            currentIndex = (currentIndex == 0) ? musicFiles.size() - 1 : currentIndex - 1;
            music.openFromFile(musicFiles[currentIndex]);
        }
        play();
    }

    void loop(bool loop) override {
        isLooping = loop;
        music.setLoop(loop);
    }

    void shuffle(bool shuffle) override {
        isShuffled = shuffle;
        if (shuffle) {
            shuffleIndices = std::vector<int>(musicFiles.size());
            for (int i = 0; i < musicFiles.size(); ++i) {
                shuffleIndices[i] = i;
            }
            std::random_shuffle(shuffleIndices.begin(), shuffleIndices.end());
            currentIndex = 0;
        }
        else {
            shuffleIndices.clear();
            currentIndex = 0;
        }
    }

    void playSong(int index) override {
        if (index >= 0 && index < musicFiles.size()) {
            currentIndex = index;
            music.openFromFile(musicFiles[currentIndex]);
            play();
        }
    }

    // Getters
    bool getIsLooping() const {
        return isLooping;
    }

    bool getIsShuffled() const {
        return isShuffled;
    }

    sf::Music::Status getStatus() const {
        return music.getStatus();
    }

private:
    sf::Music music;
    std::vector<std::string> musicFiles;
    std::vector<int> shuffleIndices;
    int currentIndex;
    bool isLooping;
    bool isShuffled;
};

int main() {
    // Get desktop mode and reduce height by a bit to avoid overlapping the taskbar
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width, desktopMode.height - 50), "SFML Music Player", sf::Style::Default);

    // Create the music player
    std::vector<std::string> musicFiles = {
        "Songs/Aparibhasit.wav",
        "Songs/High Hopes.wav",
        "Songs/Otherside.wav",
        "Songs/Purnimako Chandramalai Cover by Daya Sagar Baral.mp3",
        "Songs/Queen - I Want To Break Free (Official Video).mp3",
        "Songs/Kun Faya Kun Full Video Song Rockstar  Ranbir Kapoor  A.R. Rahman, Javed Ali, Mohit Chauhan.mp3",
        "Songs/The Weeknd - Blinding Lights (Official Video).mp3",
        "Songs/Kendrick Lamar - Not Like Us.mp3",
        "Songs/fein.mp3",
        "Songs/Creedence Clearwater Revival - Have You Ever Seen The Rain (Official).mp3"
    };
    MusicPlayer player(musicFiles);

    // Load button images
    sf::Texture playTexture, pauseTexture, nextTexture, prevTexture, shuffleTexture, loopTexture, volumeTexture;
    if (!playTexture.loadFromFile("Icons/play.png") ||
        !pauseTexture.loadFromFile("Icons/pause.png") ||
        !nextTexture.loadFromFile("Icons/skip.png") ||
        !prevTexture.loadFromFile("Icons/back.png") ||
        !shuffleTexture.loadFromFile("Icons/shuffle.png") ||
        !loopTexture.loadFromFile("Icons/loop.png") ||
        !volumeTexture.loadFromFile("Icons/volume.png")) {
        std::cerr << "Error loading images" << std::endl;
        return -1;
    }

    // Create sprites for buttons
    sf::Sprite playPauseButton(playTexture);  // Use play texture initially
    sf::Sprite nextButton(nextTexture);
    sf::Sprite prevButton(prevTexture);
    sf::Sprite shuffleButton(shuffleTexture);
    sf::Sprite loopButton(loopTexture);
    sf::Sprite volumeButton(volumeTexture);

    // Set button sizes
    float buttonWidth = 80.0f;
    float buttonHeight = 80.0f;

    playPauseButton.setScale(buttonWidth / playPauseButton.getLocalBounds().width, buttonHeight / playPauseButton.getLocalBounds().height);
    nextButton.setScale(buttonWidth / nextButton.getLocalBounds().width, buttonHeight / nextButton.getLocalBounds().height);
    prevButton.setScale(buttonWidth / prevButton.getLocalBounds().width, buttonHeight / prevButton.getLocalBounds().height);
    shuffleButton.setScale(buttonWidth / shuffleButton.getLocalBounds().width, buttonHeight / shuffleButton.getLocalBounds().height);
    loopButton.setScale(buttonWidth / loopButton.getLocalBounds().width, buttonHeight / loopButton.getLocalBounds().height);
    volumeButton.setScale(buttonWidth / volumeButton.getLocalBounds().width, buttonHeight / volumeButton.getLocalBounds().height);

    // Sidebar
    sf::RectangleShape sidebar(sf::Vector2f(200.0f, window.getSize().y));
    sidebar.setFillColor(sf::Color(30, 30, 30));

    // Main content area
    sf::RectangleShape contentArea(sf::Vector2f(window.getSize().x - 200.0f, window.getSize().y));
    contentArea.setPosition(200.0f, 0.0f);
    contentArea.setFillColor(sf::Color(40, 40, 40));

    // Media control bar
    sf::RectangleShape controlBar(sf::Vector2f(window.getSize().x, 100.0f));
    controlBar.setPosition(0.0f, window.getSize().y - 100.0f);
    controlBar.setFillColor(sf::Color(20, 20, 20));

    // Set button positions
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    float yPosition = windowHeight - buttonHeight - 10.0f;  // Adjust y position as needed

    playPauseButton.setPosition((windowWidth - buttonWidth) / 2, yPosition);  // Centered
    prevButton.setPosition(playPauseButton.getPosition().x - 2 * buttonWidth, yPosition);  // Left of play/pause
    nextButton.setPosition(playPauseButton.getPosition().x + 2 * buttonWidth, yPosition);  // Right of play/pause
    shuffleButton.setPosition(playPauseButton.getPosition().x - 4 * buttonWidth, yPosition);  // Far left
    loopButton.setPosition(playPauseButton.getPosition().x + 4 * buttonWidth, yPosition);  // Far right
    volumeButton.setPosition(windowWidth - 100, yPosition);  // Right corner

    // Search bar
    sf::RectangleShape searchBar(sf::Vector2f(180.0f, 30.0f));
    searchBar.setPosition(10.0f, 10.0f);
    searchBar.setFillColor(sf::Color(50, 50, 50));

    // Sidebar buttons and text
    sf::Font font;
    if (!font.loadFromFile("Fonts/ARIAL.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return -1;
    }

    std::vector<std::string> sidebarOptions = { "Home", "Playlists" };
    std::vector<sf::Text> sidebarTexts;

    for (size_t i = 0; i < sidebarOptions.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(sidebarOptions[i]);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.0f, 60.0f + i * 100.0f);  // Adjusted spacing
        sidebarTexts.push_back(text);
    }

    // Playlist vector
    std::vector<std::string> playlists = {
        "Favorites",
        "Chill Vibes",
        "Workout Mix"
    };

    // Start the game loop
    Page currentPage = Page::Home;
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (playPauseButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    if (player.getStatus() == sf::Music::Playing) {
                        player.pause();
                        playPauseButton.setTexture(playTexture);
                    } else {
                        player.play();
                        playPauseButton.setTexture(pauseTexture);
                    }
                }
                else if (nextButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    player.next();
                }
                else if (prevButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    player.previous();
                }
                else if (shuffleButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    player.shuffle(!player.getIsShuffled());
                }
                else if (loopButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    player.loop(!player.getIsLooping());
                }
                else if (sidebarTexts[0].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    currentPage = Page::Home;
                }
                else if (sidebarTexts[1].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    currentPage = Page::Playlists;
                }
                else if (currentPage == Page::Home) {
                    for (size_t i = 0; i < musicFiles.size(); ++i) {
                        sf::Text songText;
                        songText.setFont(font);
                        songText.setString(getBaseName(musicFiles[i]));  // Only show song names
                        songText.setCharacterSize(30);
                        songText.setFillColor(sf::Color::White);
                        songText.setPosition(220.0f, 60.0f + i * 60.0f);  // Adjusted spacing for song items

                        // Check if the mouse click is within the song text bounds
                        if (songText.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            player.playSong(i);  // Play the selected song
                            break;  // Exit the loop after playing the song
                        }
                    }
                }
            }
        }

        // Clear the screen
        window.clear();

        // Draw the sidebar
        window.draw(sidebar);
        for (const auto& text : sidebarTexts) {
            window.draw(text);
        }

        // Draw the content area
        window.draw(contentArea);

        // Draw the control bar and buttons
        window.draw(controlBar);
        window.draw(playPauseButton);
        window.draw(nextButton);
        window.draw(prevButton);
        window.draw(shuffleButton);
        window.draw(loopButton);
        window.draw(volumeButton);

        // Draw the current page content
        if (currentPage == Page::Home) {
            // Home page content: display list of song names only
            for (size_t i = 0; i < musicFiles.size(); ++i) {
                std::string songName = getBaseName(musicFiles[i]);  // Get the base name
                sf::Text songText;
                songText.setFont(font);
                songText.setString(songName);  // Use base name instead of full path
                songText.setCharacterSize(30);
                songText.setFillColor(sf::Color::White);
                songText.setPosition(220.0f, 60.0f + i * 60.0f);  // Adjusted spacing for song items
                window.draw(songText);
            }
        }
        else if (currentPage == Page::Playlists) {
            // Playlist page content
            for (size_t i = 0; i < playlists.size(); ++i) {
                sf::Text playlistText;
                playlistText.setFont(font);
                playlistText.setString(playlists[i]);
                playlistText.setCharacterSize(30);
                playlistText.setFillColor(sf::Color::White);
                playlistText.setPosition(220.0f, 60.0f + i * 100.0f);  // Adjusted spacing for playlist items
                window.draw(playlistText);
            }
        }

        // Update the window
        window.display();
    }

    return 0;
}
