#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>

enum class Page {
    Home,
    MusicLibrary,
    Playlists
};

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
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1000, 600), "SFML Music Player");

    // Create the music player
    std::vector<std::string> musicFiles = {
        "Songs/Aparibhasit.wav",
        "Songs/High Hopes.wav",
        "Songs/Otherside.wav"
    };
    MusicPlayer player(musicFiles);

    // Initialize playlists
    std::vector<std::string> playlists = {
        "Favorites",
        "Chill Vibes",
        "Workout Mix"
    };

    // Load button images
    sf::Texture playTexture, pauseTexture, nextTexture, prevTexture, shuffleTexture, loopTexture, volumeTexture, settingsTexture;
    if (!playTexture.loadFromFile("Icons/play.png") ||
        !pauseTexture.loadFromFile("Icons/pause.png") ||
        !nextTexture.loadFromFile("Icons/skip.png") ||
        !prevTexture.loadFromFile("Icons/back.png") ||
        !shuffleTexture.loadFromFile("Icons/shuffle.png") ||
        !loopTexture.loadFromFile("Icons/loop.png") ||
        !volumeTexture.loadFromFile("Icons/play.png") ||
        !settingsTexture.loadFromFile("Icons/play.png")) {
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
    sf::Sprite settingsButton(settingsTexture);

    // Set button sizes
    float buttonWidth = 40.0f;
    float buttonHeight = 40.0f;

    playPauseButton.setScale(buttonWidth / playPauseButton.getLocalBounds().width, buttonHeight / playPauseButton.getLocalBounds().height);
    nextButton.setScale(buttonWidth / nextButton.getLocalBounds().width, buttonHeight / nextButton.getLocalBounds().height);
    prevButton.setScale(buttonWidth / prevButton.getLocalBounds().width, buttonHeight / prevButton.getLocalBounds().height);
    shuffleButton.setScale(buttonWidth / shuffleButton.getLocalBounds().width, buttonHeight / shuffleButton.getLocalBounds().height);
    loopButton.setScale(buttonWidth / loopButton.getLocalBounds().width, buttonHeight / loopButton.getLocalBounds().height);
    volumeButton.setScale(buttonWidth / volumeButton.getLocalBounds().width, buttonHeight / volumeButton.getLocalBounds().height);
    settingsButton.setScale(buttonWidth / settingsButton.getLocalBounds().width, buttonHeight / settingsButton.getLocalBounds().height);

    // Sidebar
    sf::RectangleShape sidebar(sf::Vector2f(200.0f, window.getSize().y));
    sidebar.setFillColor(sf::Color(30, 30, 30));

    // Main content area
    sf::RectangleShape contentArea(sf::Vector2f(window.getSize().x - 200.0f, window.getSize().y));
    contentArea.setPosition(200.0f, 0.0f);
    contentArea.setFillColor(sf::Color(40, 40, 40));

    // Media control bar
    sf::RectangleShape controlBar(sf::Vector2f(window.getSize().x, 60.0f));
    controlBar.setPosition(0.0f, window.getSize().y - 60.0f);
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
    settingsButton.setPosition(windowWidth - 50, yPosition);  // Right corner

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
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.0f, 60.0f + i * 40.0f);
        sidebarTexts.push_back(text);
    }

    // Song buttons
    std::vector<sf::Text> songButtons;
    for (size_t i = 0; i < musicFiles.size(); ++i) {
        sf::Text songButton;
        songButton.setFont(font);
        songButton.setString(musicFiles[i]);
        songButton.setCharacterSize(20);
        songButton.setFillColor(sf::Color::White);
        songButton.setPosition(220.0f, 60.0f + i * 40.0f);
        songButtons.push_back(songButton);
    }

    // Main loop
    Page currentPage = Page::Home;
    bool isPlaying = false;
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle button clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Next button
                if (nextButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    player.next();
                }

                // Previous button
                if (prevButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    player.previous();
                }

                // Shuffle button
                if (shuffleButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    player.shuffle(!player.getIsShuffled());
                }

                // Loop button
                if (loopButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    player.loop(!player.getIsLooping());
                }

                // Play/pause button
                if (playPauseButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (isPlaying) {
                        player.pause();
                        playPauseButton.setTexture(playTexture);
                    }
                    else {
                        player.play();
                        playPauseButton.setTexture(pauseTexture);
                    }
                    isPlaying = !isPlaying;
                }

                // Sidebar options
                for (size_t i = 0; i < sidebarTexts.size(); ++i) {
                    if (sidebarTexts[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        switch (i) {
                        case 0:
                            currentPage = Page::Home;
                            break;
                        case 1:
                            currentPage = Page::Playlists;
                            break;
                        default:
                            break;
                        }
                    }
                }

                // Song buttons
                if (currentPage == Page::Home) {
                    for (size_t i = 0; i < songButtons.size(); ++i) {
                        if (songButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            player.playSong(i);
                            isPlaying = true;
                            playPauseButton.setTexture(pauseTexture);
                        }
                    }
                }
            }
        }

        // Clear screen
        window.clear();

        // Draw sidebar and main content area
        window.draw(sidebar);
        window.draw(contentArea);

        // Draw sidebar texts
        for (const auto& text : sidebarTexts) {
            window.draw(text);
        }

        // Draw media control bar and buttons
        window.draw(controlBar);
        window.draw(playPauseButton);
        window.draw(nextButton);
        window.draw(prevButton);
        window.draw(shuffleButton);
        window.draw(loopButton);
        window.draw(volumeButton);
        window.draw(settingsButton);

        // Draw content based on the current page
        if (currentPage == Page::Home) {
            // Draw song buttons
            for (const auto& button : songButtons) {
                window.draw(button);
            }
        }

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
