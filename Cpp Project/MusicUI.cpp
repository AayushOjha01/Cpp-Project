#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <iostream> // For std::cerr

struct Button {
    sf::RectangleShape shape;
    sf::Text text;
};

class MusicPlayer {
public:
    MusicPlayer() : currentTrackIndex(-1), isPlaying(false) {}

    bool loadMusic(const std::vector<std::string>& tracks) {
        for (const auto& track : tracks) {
            sf::SoundBuffer buffer;
            if (!buffer.loadFromFile(track)) {
                return false;
            }
            soundBuffers.push_back(buffer);
        }
        return true;
    }

    void playTrack(int index) {
        if (index >= 0 && index < soundBuffers.size()) {
            currentTrackIndex = index;
            music.setBuffer(soundBuffers[currentTrackIndex]);
            music.play();
            isPlaying = true;
        }
    }

    void playPause() {
        if (isPlaying) {
            music.pause();
            isPlaying = false;
        }
        else {
            music.play();
            isPlaying = true;
        }
    }

    void nextTrack() {
        if (!soundBuffers.empty()) {
            currentTrackIndex = (currentTrackIndex + 1) % soundBuffers.size();
            playTrack(currentTrackIndex);
        }
    }

    void previousTrack() {
        if (!soundBuffers.empty()) {
            currentTrackIndex = (currentTrackIndex - 1 + soundBuffers.size()) % soundBuffers.size();
            playTrack(currentTrackIndex);
        }
    }

    void stop() {
        music.stop();
        isPlaying = false;
    }

    bool isMusicPlaying() const {
        return isPlaying;
    }

private:
    sf::Sound music;
    std::vector<sf::SoundBuffer> soundBuffers;
    int currentTrackIndex;
    bool isPlaying;
};

void createButton(Button& button, const std::string& label, sf::Font& font, sf::Vector2f position, sf::Vector2f size) {
    button.shape.setSize(size);
    button.shape.setFillColor(sf::Color::Blue);
    button.shape.setOutlineColor(sf::Color::White);
    button.shape.setOutlineThickness(2);
    button.shape.setPosition(position);

    button.text.setFont(font);
    button.text.setString(label);
    button.text.setCharacterSize(20);
    button.text.setFillColor(sf::Color::White);
    button.text.setPosition(
        position.x + (size.x - button.text.getGlobalBounds().width) / 2,
        position.y + (size.y - button.text.getGlobalBounds().height) / 2 - 5
    );
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Music Player UI");

    sf::Font font;
    if (!font.loadFromFile("path/to/your/font.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    // Music Player
    MusicPlayer musicPlayer;
    std::vector<std::string> musicFiles = {
        "path/to/Aparibhasit.wav",
        "path/to/High Hopes.wav",
        "path/to/Otherside.wav"
    };

    if (!musicPlayer.loadMusic(musicFiles)) {
        std::cerr << "Error loading music files." << std::endl;
        return -1;
    }

    // Create music title buttons
    std::vector<std::string> musicTitles = { "Aparibhasit", "High Hopes", "Otherside" };
    std::vector<Button> musicButtons(musicTitles.size());

    float yOffset = 20;
    for (size_t i = 0; i < musicTitles.size(); ++i) {
        createButton(musicButtons[i], musicTitles[i], font, { 20, yOffset }, { 300, 50 });
        yOffset += 70;
    }

    // Create control buttons
    std::vector<Button> controlButtons(6);
    std::vector<std::string> controlLabels = { "Play", "Pause", "Next", "Previous", "Loop", "Shuffle" };
    float xOffset = 350;
    for (size_t i = 0; i < controlButtons.size(); ++i) {
        createButton(controlButtons[i], controlLabels[i], font, { xOffset, 20 }, { 100, 50 });
        xOffset += 120;
    }

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Event handling for music buttons
            for (size_t i = 0; i < musicButtons.size(); ++i) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (musicButtons[i].shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                        musicPlayer.playTrack(i);
                    }
                }
            }

            // Event handling for control buttons
            for (size_t i = 0; i < controlButtons.size(); ++i) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (controlButtons[i].shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                        if (controlButtons[i].text.getString() == "Play") {
                            musicPlayer.playPause();
                            controlButtons[i].text.setString("Pause");
                        }
                        else if (controlButtons[i].text.getString() == "Pause") {
                            musicPlayer.playPause();
                            controlButtons[i].text.setString("Play");
                        }
                        else if (controlButtons[i].text.getString() == "Next") {
                            musicPlayer.nextTrack();
                        }
                        else if (controlButtons[i].text.getString() == "Previous") {
                            musicPlayer.previousTrack();
                        }
                        else if (controlButtons[i].text.getString() == "Loop") {
                            // Implement loop functionality here
                        }
                        else if (controlButtons[i].text.getString() == "Shuffle") {
                            // Implement shuffle functionality here
                        }
                    }
                }
            }
        }

        window.clear();

        // Draw music buttons
        for (const auto& button : musicButtons) {
            window.draw(button.shape);
            window.draw(button.text);
        }

        // Draw control buttons
        for (const auto& button : controlButtons) {
            window.draw(button.shape);
            window.draw(button.text);
        }

        window.display();
    }

    return 0;
}
