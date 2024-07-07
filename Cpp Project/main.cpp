#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// Helper function to load a texture from file
bool loadTexture(sf::Texture& texture, const std::string& filename) {
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Error loading " << filename << std::endl;
        return false;
    }
    return true;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Music Player");

    // Load textures for buttons
    sf::Texture playTexture, pauseTexture, nextTexture, prevTexture, loopTexture, shuffleTexture;
    loadTexture(playTexture, "play.JPEG");
    loadTexture(pauseTexture, "pause.png");
    loadTexture(nextTexture, "next.png");
    loadTexture(prevTexture, "prev.png");
    loadTexture(loopTexture, "loop.png");
    loadTexture(shuffleTexture, "shuffle.png");

    // Create sprites for buttons
    sf::Sprite playPauseButton(playTexture);
    sf::Sprite nextButton(nextTexture);
    sf::Sprite prevButton(prevTexture);
    sf::Sprite loopButton(loopTexture);
    sf::Sprite shuffleButton(shuffleTexture);

    // Position buttons
    playPauseButton.setPosition(350, 500);
    nextButton.setPosition(450, 500);
    prevButton.setPosition(250, 500);
    loopButton.setPosition(150, 500);
    shuffleButton.setPosition(550, 500);

    // Load music
    sf::Music music;
    if (!music.openFromFile("Aparibhasit.wav")) {
        std::cerr << "Error loading Aparibhasit.wav" << std::endl;
        return -1;
    }

    bool isPlaying = false;
    bool loop = false;
    bool shuffle = false;
    std::vector<std::string> playlist = { "Aparibhasit.wav", "Otherside.wav", "High Hopes.wav" };
    int currentSongIndex = 0;

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (playPauseButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (isPlaying) {
                        music.pause();
                        playPauseButton.setTexture(playTexture);
                    }
                    else {
                        music.play();
                        playPauseButton.setTexture(pauseTexture);
                    }
                    isPlaying = !isPlaying;
                }

                if (nextButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    currentSongIndex = (currentSongIndex + 1) % playlist.size();
                    if (!music.openFromFile(playlist[currentSongIndex])) {
                        std::cerr << "Error loading " << playlist[currentSongIndex] << std::endl;
                    }
                    music.play();
                    playPauseButton.setTexture(pauseTexture);
                    isPlaying = true;
                }

                if (prevButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    currentSongIndex = (currentSongIndex - 1 + playlist.size()) % playlist.size();
                    if (!music.openFromFile(playlist[currentSongIndex])) {
                        std::cerr << "Error loading " << playlist[currentSongIndex] << std::endl;
                    }
                    music.play();
                    playPauseButton.setTexture(pauseTexture);
                    isPlaying = true;
                }

                if (loopButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    loop = !loop;
                    music.setLoop(loop);
                }

                if (shuffleButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    shuffle = !shuffle;
                    // Shuffle functionality can be implemented later
                }
            }
        }

        // Clear screen
        window.clear(sf::Color::White);

        // Draw UI
        window.draw(playPauseButton);
        window.draw(nextButton);
        window.draw(prevButton);
        window.draw(loopButton);
        window.draw(shuffleButton);

        // Update the window
        window.display();
    }

    return 0;
}
