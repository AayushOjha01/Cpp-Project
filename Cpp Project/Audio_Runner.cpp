#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

enum class Page {
    Home,
    MusicLibrary,
    Playlists
};

void openPlaylists(sf::RenderWindow& window, Page& currentPage, std::vector<std::string>& playlists);

int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1000, 600), "SFML Music Player");

    // Load button images (assuming these are loaded elsewhere in your code)

    // Create buttons
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

    // Variable to keep track of play/pause state
    bool isPlaying = false;

    // Variable to keep track of current page
    Page currentPage = Page::Home;

    // Example playlists
    std::vector<std::string> playlists = { "Playlist 1", "Playlist 2" };

    // Start the game loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle button clicks and sidebar clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Check sidebar clicks
                    for (size_t i = 0; i < sidebarTexts.size(); ++i) {
                        if (sidebarTexts[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                            std::cout << sidebarOptions[i] << " clicked" << std::endl;
                            if (sidebarOptions[i] == "Home") {
                                currentPage = Page::Home;
                            }
                            else if (sidebarOptions[i] == "Playlists") {
                                currentPage = Page::Playlists;
                            }
                        }
                    }

                    // Check play/pause button click
                    if (playPauseButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        // Toggle play/pause state
                        isPlaying = !isPlaying;

                        // Change button texture based on state
                        if (isPlaying) {
                            playPauseButton.setTexture(pauseTexture);
                        }
                        else {
                            playPauseButton.setTexture(playTexture);
                        }

                        // Simulate play/pause action (you can add your logic here)
                        std::cout << "Play/Pause clicked. Current state: " << (isPlaying ? "Playing" : "Paused") << std::endl;
                    }
                }
            }
        }

        // Clear screen
        window.clear();

        // Draw sidebar and content area based on current page
        switch (currentPage) {
        case Page::Home:
            // Draw home page content
            window.draw(sidebar);
            window.draw(contentArea);
            window.draw(searchBar);
            window.draw(controlBar);
            break;
        case Page::Playlists:
            // Draw playlists page content
            window.draw(sidebar);
            window.draw(contentArea);
            window.draw(searchBar);
            window.draw(controlBar);
            openPlaylists(window, currentPage, playlists);
            break;
        }

        // Draw sidebar texts (excluding current page's name)
        for (const auto& text : sidebarTexts) {
            window.draw(text);
        }

        // Draw buttons (assuming these are drawn in every page)
        window.draw(playPauseButton);
        window.draw(nextButton);
        window.draw(prevButton);
        window.draw(shuffleButton);
        window.draw(loopButton);
        window.draw(volumeButton);
        window.draw(settingsButton);

        // Update the window
        window.display();
    }

    return 0;
}

void openPlaylists(sf::RenderWindow& window, Page& currentPage, std::vector<std::string>& playlists) {
    // Sidebar
    sf::RectangleShape sidebar(sf::Vector2f(200.0f, window.getSize().y));
    sidebar.setFillColor(sf::Color(30, 30, 30));

    // Sidebar buttons and text
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return;
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

    // Example: Displaying some content in the playlists page
    // Replace this with your actual implementation
    sf::Text title;
    title.setFont(font);
    title.setString("Playlists");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::White);
    title.setPosition(220.0f, 100.0f);

    std::vector<sf::Text> playlistsTexts;
    for (size_t i = 0; i < playlists.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(playlists[i]);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
        text.setPosition(220.0f, 150.0f + i * 30.0f);
        playlistsTexts.push_back(text);
    }

    // Start playlists page loop
    while (window.isOpen() && currentPage == Page::Playlists) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // Check sidebar clicks
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (size_t i = 0; i < sidebarTexts.size(); ++i) {
                    if (sidebarTexts[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        if (sidebarOptions[i] == "Home") {
                            currentPage = Page::Home;
                        }
                        else if (sidebarOptions[i] == "Playlists") {
                            currentPage = Page::Playlists;
                        }
                    }
                }
            }
        }

        // Clear screen
        window.clear(sf::Color(40, 40, 40));

        // Draw sidebar
        window.draw(sidebar);

        // Draw sidebar texts (excluding current page's name)
        for (const auto& text : sidebarTexts) {
            window.draw(text);
        }

        // Draw playlists page content
        window.draw(title);
        for (const auto& text : playlistsTexts) {
            window.draw(text);
        }

        // Update the window
        window.display();
    }
}
