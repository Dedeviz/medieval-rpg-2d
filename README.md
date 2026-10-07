#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
constexpr unsigned int kWindowWidth = 1280;
constexpr unsigned int kWindowHeight = 720;
constexpr float kWorldSpeed = 25.0f;

enum class Screen { Menu, Options, CharacterCreate, World, Exit };

struct Stats {
    int vitalita = 1;
    int resistenza = 1;
    int cucina = 1;
    int fascino = 1;
    int armiCorte = 1;
    int armiLunghe = 1;
    int aste = 1;
    int mazze = 1;
    int scudi = 1;
    int archi = 1;
    int balestre = 1;
};

struct Character {
    std::string name = "";
    Stats stats;
    int age = 20;
    int maxHp = 100;
    int hp = 100;
    std::string spouse = "";
    std::vector<std::string> children;
    std::string title = "Nobile di Toscana";
    int wealth = 50;
    int kingdomInfluence = 0;
    int foodStock = 20;
    int fame = 0;
    int estateLevel = 1;
    sf::Vector2f position = {200.0f, 200.0f};
    sf::Color color = sf::Color::Cyan;
};

struct Npc {
    std::string name;
    sf::Vector2f position;
    std::string faction;
    std::string spouse = "";
    std::vector<std::string> children;
    int age = 18;
    bool alive = true;
    bool isLeader = false;
    int loyalty = 50;
    int mood = 50;
    sf::Color color;
    sf::Vector2f velocity;
    int aiCooldown = 0;
};

struct Button {
    sf::RectangleShape box;
    sf::Text text;
    bool active = false;
    std::string keyLabel;
};

std::vector<std::string> kFactionNames = {
    "Firenze", "Pavia", "Siena", "Venezia", "Milano", "Genova", "Bologna", "Napoli"
};

std::vector<std::string> kFirstNames = {
    "Lorenzo", "Guglielmo", "Matteo", "Alessandro", "Pietro", "Giovanni", "Adriano", "Riccardo",
    "Caterina", "Beatrice", "Elena", "Margherita", "Lucia", "Giulia", "Anita", "Matilda"
};

std::vector<std::string> kLastNames = {
    "de' Medici", "da Vinci", "Della Rovere", "Bardi", "Visconti", "Gonzaga", "Farnese", "Sforza",
    "Barbaro", "Bonaparte", "Bolognini", "Tornabuoni", "D'Este", "Bocchi", "Aldobrandini"
};

std::string pickRandom(const std::vector<std::string>& items) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(items.size()) - 1);
    return items[dist(rng)];
}

std::string makeNpcName() {
    return pickRandom(kFirstNames) + " " + pickRandom(kLastNames);
}

char toUpperAscii(char c) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

std::string titleCase(std::string value) {
    if (value.empty()) return value;
    value[0] = toUpperAscii(value[0]);
    for (size_t i = 1; i < value.size(); ++i) {
        if (value[i - 1] == ' ') value[i] = toUpperAscii(value[i]);
    }
    return value;
}

std::string trim(const std::string& text) {
    const auto begin = text.find_first_not_of(" \n\r\t");
    if (begin == std::string::npos) return "";
    const auto end = text.find_last_not_of(" \n\r\t");
    return text.substr(begin, end - begin + 1);
}

std::string toString(int value) {
    return std::to_string(value);
}

class Game {
public:
    Game() : window(sf::VideoMode(kWindowWidth, kWindowHeight), "1223: Italian Kingdoms", sf::Style::Close) {
        window.setFramerateLimit(60);
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
            std::cout << "Font not found; using default font." << std::endl;
        }
        setupMenu();
        setupOptions();
        setupCharacterCreation();
        resetWorld();
    }

    void run() {
        while (window.isOpen()) {
            processEvents();
            update();
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::Font font;
    Screen screen = Screen::Menu;
    std::vector<Button> menuButtons;
    std::vector<Button> optionButtons;
    std::vector<Button> statButtons;
    std::vector<std::string> heroNames;

    Character player;
    std::vector<Npc> npcs;
    sf::Vector2f camera = {0.0f, 0.0f};
    float worldClock = 0.0f;
    bool worldReady = false;
    int selectedStatIndex = 0;
    int statPointsLeft = 20;
    bool showNamePrompt = false;
    std::string inputName;
    int selectedResolution = 1;
    std::vector<std::pair<unsigned int, unsigned int>> resolutions = {
        {960, 540},
        {1280, 720},
        {1600, 900}
    };

    void setupMenu() {
        menuButtons.clear();
        const std::array<std::string, 3> labels = {"Nuova Partita", "Opzioni", "Esci"};
        for (size_t i = 0; i < labels.size(); ++i) {
            Button b;
            b.box.setSize({360.0f, 58.0f});
            b.box.setPosition({450.0f, 280.0f + static_cast<float>(i) * 90.0f});
            b.box.setFillColor(i == 0 ? sf::Color(199, 156, 80) : sf::Color(52, 42, 32));
            b.box.setOutlineColor(sf::Color::Black);
            b.box.setOutlineThickness(2.0f);
            b.text.setFont(font);
            b.text.setString(labels[i]);
            b.text.setCharacterSize(26);
            b.text.setFillColor(sf::Color::White);
            b.text.setPosition({b.box.getPosition().x + 24.0f, b.box.getPosition().y + 10.0f});
            b.active = (i == 0);
            b.keyLabel = labels[i];
            menuButtons.push_back(b);
        }
    }

    void setupOptions() {
        optionButtons.clear();
        const std::array<std::string, 3> labels = {"1920x1080", "1280x720", "960x540"};
        for (size_t i = 0; i < labels.size(); ++i) {
            Button b;
            b.box.setSize({260.0f, 48.0f});
            b.box.setPosition({500.0f, 260.0f + static_cast<float>(i) * 70.0f});
            b.box.setFillColor(i == selectedResolution ? sf::Color(199, 156, 80) : sf::Color(64, 54, 43));
            b.box.setOutlineColor(sf::Color::Black);
            b.box.setOutlineThickness(1.5f);
            b.text.setFont(font);
            b.text.setString(labels[i]);
            b.text.setCharacterSize(22);
            b.text.setFillColor(sf::Color::White);
            b.text.setPosition({b.box.getPosition().x + 20.0f, b.box.getPosition().y + 8.0f});
            b.active = (i == selectedResolution);
            optionButtons.push_back(b);
        }
    }

    void setupCharacterCreation() {
        statButtons.clear();
        const std::array<std::pair<std::string, int*>, 11> statTargets = {
            std::make_pair("VITALITA", &player.stats.vitalita),
            std::make_pair("RESISTENZA", &player.stats.resistenza),
            std::make_pair("CUCINA", &player.stats.cucina),
            std::make_pair("FASCINO", &player.stats.fascino),
            std::make_pair("ARMI CORTE", &player.stats.armiCorte),
            std::make_pair("ARMI LUNGHE", &player.stats.armiLunghe),
            std::make_pair("ASTE", &player.stats.aste),
            std::make_pair("MAZZE", &player.stats.mazze),
            std::make_pair("SCUDI", &player.stats.scudi),
            std::make_pair("ARCHI", &player.stats.archi),
            std::make_pair("BALLESTRE", &player.stats.balestre)
        };

        for (size_t i = 0; i < statTargets.size(); ++i) {
            Button b;
            b.box.setSize({420.0f, 32.0f});
            b.box.setPosition({420.0f, 140.0f + static_cast<float>(i) * 38.0f});
            b.box.setFillColor(sf::Color(52, 40, 32));
            b.box.setOutlineColor(sf::Color::Black);
            b.box.setOutlineThickness(1.0f);
            b.text.setFont(font);
            b.text.setCharacterSize(18);
            b.text.setFillColor(sf::Color::White);
            b.text.setPosition({b.box.getPosition().x + 12.0f, b.box.getPosition().y + 3.0f});
            b.keyLabel = statTargets[i].first;
            statButtons.push_back(b);
        }
    }

    void resetWorld() {
        worldReady = true;
        npcs.clear();
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> distX(60.0f, 1000.0f);
        std::uniform_real_distribution<float> distY(60.0f, 600.0f);

        for (int i = 0; i < 18; ++i) {
            Npc npc;
            npc.name = makeNpcName();
            npc.faction = pickRandom(kFactionNames);
            npc.position = {distX(rng), distY(rng)};
            npc.age = 18 + (rng() % 35);
            npc.color = sf::Color((rng() % 255), (rng() % 200), (rng() % 170));
            npc.velocity = {((rng() % 2 == 0) ? 1.0f : -1.0f) * (0.3f + float(rng() % 50) / 100.0f), ((rng() % 2 == 0) ? 1.0f : -1.0f) * (0.3f + float(rng() % 50) / 100.0f)};
            npc.loyalty = 20 + (rng() % 60);
            npc.mood = 30 + (rng() % 60);
            npc.aiCooldown = rng() % 120;
            if (i % 4 == 0) npc.isLeader = true;
            npcs.push_back(npc);
        }
    }

    void setScreen(Screen nextScreen) {
        screen = nextScreen;
    }

    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (screen == Screen::Menu) {
                    if (event.key.code == sf::Keyboard::Up) {
                        moveMenuSelection(-1);
                    } else if (event.key.code == sf::Keyboard::Down) {
                        moveMenuSelection(1);
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        if (menuButtons[0].active) {
                            setScreen(Screen::CharacterCreate);
                        } else if (menuButtons[1].active) {
                            setScreen(Screen::Options);
                        } else {
                            window.close();
                        }
                    }
                } else if (screen == Screen::Options) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedResolution = (selectedResolution + 2) % 3;
                        setupOptions();
                    } else if (event.key.code == sf::Keyboard::Down) {
                        selectedResolution = (selectedResolution + 1) % 3;
                        setupOptions();
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        auto [w, h] = resolutions[selectedResolution];
                        window.create(sf::VideoMode(w, h), "1223: Italian Kingdoms", sf::Style::Close);
                        setScreen(Screen::Menu);
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        setScreen(Screen::Menu);
                    }
                } else if (screen == Screen::CharacterCreate) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedStatIndex = (selectedStatIndex + statButtons.size() - 1) % statButtons.size();
                    } else if (event.key.code == sf::Keyboard::Down) {
                        selectedStatIndex = (selectedStatIndex + 1) % statButtons.size();
                    } else if (event.key.code == sf::Keyboard::Left) {
                        spendPoint(-1);
                    } else if (event.key.code == sf::Keyboard::Right) {
                        spendPoint(1);
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        if (statPointsLeft == 0) {
                            finalizeCharacter();
                            setScreen(Screen::World);
                        }
                    } else if (event.key.code == sf::Keyboard::BackSpace && !inputName.empty()) {
                        inputName.pop_back();
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        setScreen(Screen::Menu);
                    } else if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z) {
                        inputName.push_back(static_cast<char>(event.key.code + 65));
                    }
                } else if (screen == Screen::World) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        setScreen(Screen::Menu);
                    }
                }
            }
        }
    }

    void moveMenuSelection(int delta) {
        int activeIndex = -1;
        for (size_t i = 0; i < menuButtons.size(); ++i) {
            if (menuButtons[i].active) {
                activeIndex = static_cast<int>(i);
                menuButtons[i].active = false;
                break;
            }
        }

        int nextIndex = activeIndex == -1 ? 0 : (activeIndex + delta + static_cast<int>(menuButtons.size())) % menuButtons.size();
        menuButtons[nextIndex].active = true;
    }

    void spendPoint(int delta) {
        const auto& statMapping = std::array<int*, 11>{
            &player.stats.vitalita,
            &player.stats.resistenza,
            &player.stats.cucina,
            &player.stats.fascino,
            &player.stats.armiCorte,
            &player.stats.armiLunghe,
            &player.stats.aste,
            &player.stats.mazze,
            &player.stats.scudi,
            &player.stats.archi,
            &player.stats.balestre
        };

        if (delta > 0 && statPointsLeft > 0) {
            *statMapping[selectedStatIndex] += 1;
            --statPointsLeft;
        } else if (delta < 0 && *statMapping[selectedStatIndex] > 1) {
            *statMapping[selectedStatIndex] -= 1;
            ++statPointsLeft;
        }
    }

    void finalizeCharacter() {
        player.name = trim(inputName.empty() ? "Lorenzo" : inputName);
        player.title = "Barone del Regno d'Italia";
        player.maxHp = 45 + player.stats.vitalita * 6;
        player.hp = player.maxHp;
        player.age = 20;
        player.position = {200.0f, 220.0f};
        player.color = sf::Color::Magenta;
        player.wealth = 80 + player.stats.cucina * 10;
        player.fame = 5 + player.stats.fascino * 2;

        if (player.stats.armiCorte >= 4) player.title = "Cavaliere di Spada Corta";
        if (player.stats.archi >= 4) player.title = "Arciere di Casa Reale";
        if (player.stats.balestre >= 4) player.title = "Capitano delle Balestre";
        if (player.stats.cucina >= 5) player.title = "Maison del Re";
    }

    void update() {
        if (screen == Screen::World) {
            updateWorld();
        }
    }

    void updateWorld() {
        const float dt = 1.0f / 60.0f;
        worldClock += dt * kWorldSpeed;

        float dx = 0.0f;
        float dy = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dy -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dy += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dx -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dx += 1.0f;

        if (dx != 0.0f || dy != 0.0f) {
            float length = std::sqrt(dx * dx + dy * dy);
            dx /= length;
            dy /= length;
            player.position.x += dx * 2.2f;
            player.position.y += dy * 2.2f;
        }

        for (auto& npc : npcs) {
            if (!npc.alive) continue;
            npc.aiCooldown = std::max(0, npc.aiCooldown - 1);
            if (npc.aiCooldown == 0) {
                npc.position += npc.velocity * 0.9f;
                if (npc.position.x < 40.0f || npc.position.x > 1100.0f) npc.velocity.x *= -1.0f;
                if (npc.position.y < 40.0f || npc.position.y > 660.0f) npc.velocity.y *= -1.0f;
                npc.aiCooldown = 40 + (rand() % 80);
            }

            if (npc.age >= 18 && npc.spouse.empty() && (rand() % 5000) == 0) {
                npc.spouse = makeNpcName();
            }

            if (npc.age >= 18 && !npc.spouse.empty() && npc.children.size() < 4 && (rand() % 8000) == 0) {
                npc.children.push_back("figlio");
            }

            if (npc.age >= 14 && npc.children.size() > 0 && npc.isLeader && (rand() % 20000) == 0) {
                npc.isLeader = false;
            }
        }

        camera.x = player.position.x - 460.0f;
        camera.y = player.position.y - 260.0f;
        camera.x = std::clamp(camera.x, 0.0f, 700.0f);
        camera.y = std::clamp(camera.y, 0.0f, 420.0f);
    }

    void render() {
        window.clear(sf::Color(17, 15, 15));

        if (screen == Screen::Menu) {
            drawMenu();
        } else if (screen == Screen::Options) {
            drawOptions();
        } else if (screen == Screen::CharacterCreate) {
            drawCharacterCreation();
        } else if (screen == Screen::World) {
            drawWorld();
        }

        window.display();
    }

    void drawMenu() {
        sf::Text title;
        title.setFont(font);
        title.setString("1223: Italian Kingdoms");
        title.setCharacterSize(52);
        title.setFillColor(sf::Color(235, 204, 117));
        title.setPosition(300.0f, 70.0f);
        window.draw(title);

        sf::Text subtitle;
        subtitle.setFont(font);
        subtitle.setString("Italia medievale, dinastie, fato, commercio e guerra");
        subtitle.setCharacterSize(20);
        subtitle.setFillColor(sf::Color(227, 220, 190));
        subtitle.setPosition(245.0f, 150.0f);
        window.draw(subtitle);

        for (const auto& b : menuButtons) {
            window.draw(b.box);
            window.draw(b.text);
        }

        sf::Text controls;
        controls.setFont(font);
        controls.setString("WASD = movimento\nE = interagisci\nSPAZIO = attacco\n1,2,3 = arma\nESC = menu");
        controls.setCharacterSize(18);
        controls.setFillColor(sf::Color(186, 219, 255));
        controls.setPosition(60.0f, 580.0f);
        window.draw(controls);
    }

    void drawOptions() {
        sf::Text title;
        title.setFont(font);
        title.setString("Opzioni grafiche");
        title.setCharacterSize(42);
        title.setPosition(440.0f, 40.0f);
        title.setFillColor(sf::Color(235, 204, 117));
        window.draw(title);

        for (const auto& b : optionButtons) {
            window.draw(b.box);
            window.draw(b.text);
        }

        sf::Text info;
        info.setFont(font);
        info.setString("Scegli la risoluzione. Premi Invio per confermare.");
        info.setCharacterSize(20);
        info.setFillColor(sf::Color(240, 230, 190));
        info.setPosition(340.0f, 600.0f);
        window.draw(info);
    }

    void drawCharacterCreation() {
        sf::Text title;
        title.setFont(font);
        title.setString("Creazione del personaggio");
        title.setCharacterSize(34);
        title.setPosition(410.0f, 30.0f);
        title.setFillColor(sf::Color(235, 204, 117));
        window.draw(title);

        sf::Text inputLabel;
        inputLabel.setFont(font);
        inputLabel.setString("Nome del protagonista");
        inputLabel.setCharacterSize(22);
        inputLabel.setPosition(140.0f, 90.0f);
        inputLabel.setFillColor(sf::Color(230, 220, 180));
        window.draw(inputLabel);

        sf::RectangleShape inputBox({520.0f, 40.0f});
        inputBox.setPosition(140.0f, 120.0f);
        inputBox.setFillColor(sf::Color(60, 48, 40));
        inputBox.setOutlineColor(sf::Color(199, 156, 80));
        inputBox.setOutlineThickness(2.0f);
        window.draw(inputBox);

        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(inputName.empty() ? "Lorenzo" : inputName);
        nameText.setCharacterSize(22);
        nameText.setPosition(160.0f, 128.0f);
        nameText.setFillColor(sf::Color::White);
        window.draw(nameText);

        sf::Text pointsText;
        pointsText.setFont(font);
        pointsText.setString("Punti disponibili: " + std::to_string(statPointsLeft));
        pointsText.setCharacterSize(22);
        pointsText.setPosition(760.0f, 90.0f);
        pointsText.setFillColor(sf::Color(204, 233, 255));
        window.draw(pointsText);

        const auto& statMapping = std::array<std::pair<std::string, int>, 11>{
            { {"VITALITA", player.stats.vitalita}, {"RESISTENZA", player.stats.resistenza}, {"CUCINA", player.stats.cucina},
              {"FASCINO", player.stats.fascino}, {"ARMI CORTE", player.stats.armiCorte}, {"ARMI LUNGHE", player.stats.armiLunghe},
              {"ASTE", player.stats.aste}, {"MAZZE", player.stats.mazze}, {"SCUDI", player.stats.scudi},
              {"ARCHI", player.stats.archi}, {"BALLESTRE", player.stats.balestre} }
        };

        for (size_t i = 0; i < statMapping.size(); ++i) {
            sf::RectangleShape bar({ 380.0f, 26.0f });
            bar.setPosition({ 440.0f, 170.0f + static_cast<float>(i) * 40.0f });
            bar.setFillColor(i == static_cast<size_t>(selectedStatIndex) ? sf::Color(74, 64, 39) : sf::Color(46, 40, 32));
            bar.setOutlineColor(sf::Color::Black);
            window.draw(bar);

            sf::RectangleShape valueBar({ static_cast<float>(statMapping[i].second * 24.0f), 24.0f });
            valueBar.setPosition(bar.getPosition().x + 2.0f, bar.getPosition().y + 1.0f);
            valueBar.setFillColor(sf::Color(199, 156, 80));
            window.draw(valueBar);

            sf::Text label;
            label.setFont(font);
            label.setString(statMapping[i].first + "   " + std::to_string(statMapping[i].second));
            label.setCharacterSize(16);
            label.setPosition({bar.getPosition().x + 12.0f, bar.getPosition().y + 1.0f});
            label.setFillColor(sf::Color::White);
            window.draw(label);
        }

        sf::Text helper;
        helper.setFont(font);
        helper.setString("Freccette: modifica. Invio: conferma. Nomi: digitare e cancellare con backspace.");
        helper.setCharacterSize(18);
        helper.setFillColor(sf::Color(180, 220, 255));
        helper.setPosition(220.0f, 650.0f);
        window.draw(helper);
    }

    void drawWorld() {
        sf::RectangleShape sky({1280.0f, 720.0f});
        sky.setFillColor(sf::Color(35, 26, 17));
        window.draw(sky);

        float cycle = std::fmod(worldClock, 24.0f);
        float nightFactor = 0.35f + 0.65f * std::sin((cycle / 24.0f) * 3.14159f);
        sf::Color dayTint(45, 55, 35);
        sf::Color duskTint(95, 65, 40);
        sf::Color finalTint = sf::Color(
            static_cast<sf::Uint8>(dayTint.r + (duskTint.r - dayTint.r) * (1.0f - nightFactor)),
            static_cast<sf::Uint8>(dayTint.g + (duskTint.g - dayTint.g) * (1.0f - nightFactor)),
            static_cast<sf::Uint8>(dayTint.b + (duskTint.b - dayTint.b) * (1.0f - nightFactor))
        );

        sf::RectangleShape overlay({1280.0f, 720.0f});
        overlay.setFillColor(sf::Color(finalTint.r, finalTint.g, finalTint.b, 135));
        window.draw(overlay);

        sf::RectangleShape mapRect({1000.0f, 700.0f});
        mapRect.setPosition({20.0f, 10.0f});
        mapRect.setFillColor(sf::Color(42, 59, 35));
        window.draw(mapRect);

        for (int x = 0; x < 40; ++x) {
            for (int y = 0; y < 30; ++y) {
                sf::RectangleShape tile({24.0f, 24.0f});
                tile.setPosition({20.0f + x * 24.0f - camera.x, 10.0f + y * 24.0f - camera.y});
                tile.setFillColor((x + y) % 2 == 0 ? sf::Color(62, 82, 48) : sf::Color(52, 70, 42));
                window.draw(tile);
            }
        }

        for (const auto& npc : npcs) {
            sf::CircleShape circle(8.0f);
            circle.setFillColor(npc.color);
            circle.setPosition(npc.position.x - camera.x, npc.position.y - camera.y);
            circle.setOrigin(8.0f, 8.0f);
            window.draw(circle);

            sf::Text label;
            label.setFont(font);
            label.setString(npc.name.substr(0, 9));
            label.setCharacterSize(12);
            label.setFillColor(sf::Color::White);
            label.setPosition(npc.position.x - camera.x + 12.0f, npc.position.y - camera.y - 18.0f);
            window.draw(label);
        }

        sf::CircleShape hero(12.0f);
        hero.setFillColor(player.color);
        hero.setPosition(player.position.x - camera.x, player.position.y - camera.y);
        hero.setOrigin(12.0f, 12.0f);
        window.draw(hero);

        sf::Text hud;
        hud.setFont(font);
        std::ostringstream hudStream;
        hudStream << "Nome: " << player.name << "\nTitolo: " << player.title << "\nEt\xe0: " << player.age
                  << "\nHP: " << player.hp << "/" << player.maxHp << "\nFazione: " << kFactionNames[0]
                  << "\nGiorno: " << static_cast<int>(worldClock / 24.0f) + 1;
        hud.setString(hudStream.str());
        hud.setCharacterSize(18);
        hud.setFillColor(sf::Color::White);
        hud.setPosition(1040.0f, 30.0f);
        window.draw(hud);

        sf::Text intent;
        intent.setFont(font);
        intent.setString("Regole sociali 1223: matrimonio, figli, successione e tutela dei minori.\nFiglio > 14 anni pu\xf2 ereditare e guidare.\nMatrimonio e famiglie si formano in modo realistico.");
        intent.setCharacterSize(16);
        intent.setFillColor(sf::Color(248, 209, 128));
        intent.setPosition(20.0f, 650.0f);
        window.draw(intent);
    }
};

}  // namespace

int main() {
    Game game;
    game.run();
    return 0;
}
