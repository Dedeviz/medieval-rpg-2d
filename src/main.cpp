#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr unsigned int kWindowWidth = 1280;
constexpr unsigned int kWindowHeight = 720;
constexpr float kTimeSpeed = 25.0f;

struct CharacterStats {
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
    std::string name = "Lorenzo";
    std::string title = "Nobile di Toscana";
    CharacterStats stats;
    int age = 20;
    int maxHp = 80;
    int hp = 80;
    int gold = 50;
    int fame = 5;
    int food = 25;
    int influence = 0;
    std::string spouse = "";
    std::vector<std::string> children;
    sf::Vector2f position{200.0f, 220.0f};
    sf::Color color{255, 204, 102};
};

struct NPC {
    std::string name;
    std::string faction;
    std::string spouse;
    std::vector<std::string> children;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    int age = 18;
    int loyalty = 50;
    int mood = 50;
    bool alive = true;
    bool leader = false;
    int aiTimer = 0;
};

enum class Screen {
    Menu,
    Options,
    CharacterCreation,
    World,
    Exit
};

struct MenuButton {
    sf::RectangleShape box;
    sf::Text label;
    bool active = false;
};

std::string toUpperWord(std::string value) {
    if (value.empty()) return value;
    value[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(value[0])));
    for (size_t i = 1; i < value.size(); ++i) {
        if (value[i - 1] == ' ') {
            value[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(value[i])));
        }
    }
    return value;
}

std::string trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \n\r\t");
    if (start == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \n\r\t");
    return s.substr(start, end - start + 1);
}

std::string randomName() {
    static const std::vector<std::string> first = {
        "Lorenzo", "Guglielmo", "Matteo", "Pietro", "Giovanni", "Riccardo",
        "Alessandro", "Rinaldo", "Caterina", "Beatrice", "Lucia", "Elena",
        "Margherita", "Matilda", "Giulia", "Adriana"
    };
    static const std::vector<std::string> last = {
        "de' Medici", "da Vinci", "Della Rovere", "Bardi", "Visconti",
        "Sforza", "Farnese", "Gonzaga", "Barbaro", "Tornabuoni", "D'Este"
    };
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> d1(0, static_cast<int>(first.size()) - 1);
    std::uniform_int_distribution<int> d2(0, static_cast<int>(last.size()) - 1);
    return first[d1(rng)] + " " + last[d2(rng)];
}

std::string randomFaction() {
    static const std::vector<std::string> factions = {
        "Firenze", "Siena", "Venezia", "Milano", "Genova", "Pavia", "Bologna", "Napoli"
    };
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> d(0, static_cast<int>(factions.size()) - 1);
    return factions[d(rng)];
}

std::vector<std::string> kItalianNames = {
    "Lorenzo", "Guglielmo", "Matteo", "Pietro", "Giovanni", "Riccardo",
    "Caterina", "Beatrice", "Lucia", "Elena", "Margherita", "Giulia"
};

class Game {
public:
    Game()
        : window(sf::VideoMode(kWindowWidth, kWindowHeight), "1223: Italian Kingdoms", sf::Style::Close) {
        window.setFramerateLimit(60);
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
            std::cerr << "Font not found: using default fallback\n";
        }
        buildMenuButtons();
        buildResolutionButtons();
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
    Screen currentScreen = Screen::Menu;

    std::vector<MenuButton> menuButtons;
    std::vector<MenuButton> optionButtons;
    std::array<std::string, 11> statLabels = {
        "Vitalita", "Resistenza", "Cucina", "Fascino", "Armi corte",
        "Armi lunghe", "Aste", "Mazze", "Scudi", "Archi", "Balestre"
    };

    int selectedStatIndex = 0;
    int statPoints = 20;
    std::string playerInputName;
    Character player;
    std::vector<NPC> npcs;
    sf::Vector2f camera{0.f, 0.f};
    float worldTime = 0.f;
    int selectedResolutionIndex = 1;
    std::vector<std::pair<unsigned int, unsigned int>> resolutions = {
        {1280, 720}, {1600, 900}, {960, 540}
    };

    void buildMenuButtons() {
        menuButtons.clear();
        const std::array<std::string, 3> labels = {"Nuova Partita", "Opzioni", "Esci"};
        for (size_t i = 0; i < labels.size(); ++i) {
            MenuButton b;
            b.box.setSize({340.f, 56.f});
            b.box.setPosition({470.f, 290.f + static_cast<float>(i) * 80.f});
            b.box.setFillColor((i == 0) ? sf::Color(190, 150, 69) : sf::Color(50, 40, 35));
            b.box.setOutlineColor(sf::Color::Black);
            b.box.setOutlineThickness(2.f);
            b.label.setFont(font);
            b.label.setString(labels[i]);
            b.label.setCharacterSize(28);
            b.label.setFillColor(sf::Color::White);
            b.label.setPosition({b.box.getPosition().x + 22.f, b.box.getPosition().y + 12.f});
            b.active = (i == 0);
            menuButtons.push_back(b);
        }
    }

    void buildResolutionButtons() {
        optionButtons.clear();
        const std::array<std::string, 3> labels = {"1280x720", "1600x900", "960x540"};
        for (size_t i = 0; i < labels.size(); ++i) {
            MenuButton b;
            b.box.setSize({260.f, 46.f});
            b.box.setPosition({480.f, 260.f + static_cast<float>(i) * 70.f});
            b.box.setFillColor(i == selectedResolutionIndex ? sf::Color(190, 150, 69) : sf::Color(64, 52, 43));
            b.box.setOutlineColor(sf::Color::Black);
            b.box.setOutlineThickness(1.5f);
            b.label.setFont(font);
            b.label.setString(labels[i]);
            b.label.setCharacterSize(22);
            b.label.setFillColor(sf::Color::White);
            b.label.setPosition({b.box.getPosition().x + 18.f, b.box.getPosition().y + 8.f});
            optionButtons.push_back(b);
        }
    }

    void resetWorld() {
        npcs.clear();
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> xDist(50.f, 1000.f);
        std::uniform_real_distribution<float> yDist(50.f, 620.f);

        for (int i = 0; i < 18; ++i) {
            NPC npc;
            npc.name = randomName();
            npc.faction = randomFaction();
            npc.position = {xDist(rng), yDist(rng)};
            npc.velocity = {
                ((rng() % 2 == 0) ? 1.f : -1.f) * (0.5f + (rng() % 40) / 100.f),
                ((rng() % 2 == 0) ? 1.f : -1.f) * (0.5f + (rng() % 40) / 100.f)
            };
            npc.color = sf::Color(rng() % 255, rng() % 200, rng() % 180);
            npc.age = 18 + (rng() % 40);
            npc.loyalty = 30 + (rng() % 60);
            npc.mood = 30 + (rng() % 60);
            npc.aiTimer = 30 + (rng() % 120);
            npc.leader = (i % 4 == 0);
            npcs.push_back(npc);
        }
    }

    void finalizeCharacter() {
        player.name = trim(playerInputName.empty() ? "Lorenzo" : playerInputName);
        player.name = toUpperWord(player.name);
        player.maxHp = 45 + player.stats.vitalita * 8;
        player.hp = player.maxHp;
        player.age = 20;
        player.gold = 80 + player.stats.cucina * 8;
        player.fame = 4 + player.stats.fascino * 2;
        player.food = 30;
        player.influence = 5 + player.stats.resistenza;
        player.title = "Nobile di Toscana";

        if (player.stats.armiCorte >= 4) player.title = "Cavaliere di Spada Corta";
        if (player.stats.archi >= 4) player.title = "Arciere della Casa";
        if (player.stats.balestre >= 4) player.title = "Capitano delle Balestre";
        if (player.stats.cucina >= 5) player.title = "Mastro di Casa";
    }

    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (currentScreen == Screen::Menu) {
                    if (event.key.code == sf::Keyboard::Up) {
                        moveMenu(-1);
                    } else if (event.key.code == sf::Keyboard::Down) {
                        moveMenu(1);
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        if (menuButtons[0].active) {
                            currentScreen = Screen::CharacterCreation;
                        } else if (menuButtons[1].active) {
                            currentScreen = Screen::Options;
                        } else {
                            window.close();
                        }
                    }
                } else if (currentScreen == Screen::Options) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedResolutionIndex = (selectedResolutionIndex + 2) % 3;
                        buildResolutionButtons();
                    } else if (event.key.code == sf::Keyboard::Down) {
                        selectedResolutionIndex = (selectedResolutionIndex + 1) % 3;
                        buildResolutionButtons();
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        auto [w, h] = resolutions[selectedResolutionIndex];
                        window.create(sf::VideoMode(w, h), "1223: Italian Kingdoms", sf::Style::Close);
                        currentScreen = Screen::Menu;
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        currentScreen = Screen::Menu;
                    }
                } else if (currentScreen == Screen::CharacterCreation) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedStatIndex = (selectedStatIndex + static_cast<int>(statLabels.size()) - 1) % statLabels.size();
                    } else if (event.key.code == sf::Keyboard::Down) {
                        selectedStatIndex = (selectedStatIndex + 1) % statLabels.size();
                    } else if (event.key.code == sf::Keyboard::Left) {
                        adjustStat(-1);
                    } else if (event.key.code == sf::Keyboard::Right) {
                        adjustStat(1);
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        if (statPoints == 0) {
                            finalizeCharacter();
                            currentScreen = Screen::World;
                            resetWorld();
                        }
                    } else if (event.key.code == sf::Keyboard::BackSpace && !playerInputName.empty()) {
                        playerInputName.pop_back();
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        currentScreen = Screen::Menu;
                    } else if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z) {
                        char c = static_cast<char>(event.key.code - sf::Keyboard::A + 'A');
                        playerInputName += c;
                    }
                } else if (currentScreen == Screen::World) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        currentScreen = Screen::Menu;
                    }
                }
            }
        }
    }

    void moveMenu(int delta) {
        int current = -1;
        for (size_t i = 0; i < menuButtons.size(); ++i) {
            if (menuButtons[i].active) {
                current = static_cast<int>(i);
                menuButtons[i].active = false;
                break;
            }
        }
        current = (current == -1) ? 0 : current;
        int target = (current + delta + static_cast<int>(menuButtons.size())) % static_cast<int>(menuButtons.size());
        menuButtons[target].active = true;
    }

    void adjustStat(int delta) {
        auto setValue = [&](int& value) {
            if (delta > 0 && statPoints > 0) {
                ++value;
                --statPoints;
            } else if (delta < 0 && value > 1) {
                --value;
                ++statPoints;
            }
        };

        switch (selectedStatIndex) {
            case 0: setValue(player.stats.vitalita); break;
            case 1: setValue(player.stats.resistenza); break;
            case 2: setValue(player.stats.cucina); break;
            case 3: setValue(player.stats.fascino); break;
            case 4: setValue(player.stats.armiCorte); break;
            case 5: setValue(player.stats.armiLunghe); break;
            case 6: setValue(player.stats.aste); break;
            case 7: setValue(player.stats.mazze); break;
            case 8: setValue(player.stats.scudi); break;
            case 9: setValue(player.stats.archi); break;
            case 10: setValue(player.stats.balestre); break;
            default: break;
        }
    }

    void update() {
        if (currentScreen == Screen::World) {
            updateWorld();
        }
    }

    void updateWorld() {
        const float dt = 1.0f / 60.0f;
        worldTime += dt * kTimeSpeed;

        float moveX = 0.f;
        float moveY = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveY -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveY += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveX -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveX += 1.f;

        if (moveX != 0.f || moveY != 0.f) {
            float length = std::sqrt(moveX * moveX + moveY * moveY);
            moveX /= length;
            moveY /= length;
            player.position.x += moveX * 2.2f;
            player.position.y += moveY * 2.2f;
        }

        for (auto& npc : npcs) {
            npc.aiTimer--;
            if (npc.aiTimer <= 0) {
                npc.position += npc.velocity * 0.8f;
                npc.aiTimer = 25 + (rand() % 80);
                if (npc.position.x < 40.f || npc.position.x > 1000.f) npc.velocity.x *= -1.f;
                if (npc.position.y < 40.f || npc.position.y > 650.f) npc.velocity.y *= -1.f;
            }
            if (npc.age >= 18 && npc.spouse.empty() && (rand() % 2000) == 0) {
                npc.spouse = randomName();
            }
            if (npc.age >= 18 && !npc.spouse.empty() && npc.children.size() < 4 && (rand() % 4000) == 0) {
                npc.children.push_back("figlio");
            }
        }

        camera.x = player.position.x - 440.f;
        camera.y = player.position.y - 260.f;
        camera.x = std::clamp(camera.x, 0.f, 700.f);
        camera.y = std::clamp(camera.y, 0.f, 360.f);
    }

    void render() {
        window.clear(sf::Color(22, 17, 14));

        if (currentScreen == Screen::Menu) {
            drawMenu();
        } else if (currentScreen == Screen::Options) {
            drawOptions();
        } else if (currentScreen == Screen::CharacterCreation) {
            drawCharacterCreation();
        } else if (currentScreen == Screen::World) {
            drawWorld();
        }

        window.display();
    }

    void drawMenu() {
        sf::Text title;
        title.setFont(font);
        title.setString("1223: Italian Kingdoms");
        title.setCharacterSize(54);
        title.setFillColor(sf::Color(228, 188, 95));
        title.setPosition({255.f, 70.f});
        window.draw(title);

        sf::Text subtitle;
        subtitle.setFont(font);
        subtitle.setString("Italia medievale, famiglie, fazioni, commercio e destino");
        subtitle.setCharacterSize(22);
        subtitle.setFillColor(sf::Color(220, 214, 183));
        subtitle.setPosition({255.f, 145.f});
        window.draw(subtitle);

        for (const auto& bt : menuButtons) {
            window.draw(bt.box);
            window.draw(bt.label);
        }

        sf::Text help;
        help.setFont(font);
        help.setString("WASD = movimento\nE = interagisci\nSpazio = attacco\nEsc = menu\n1/2/3 = armi");
        help.setCharacterSize(18);
        help.setFillColor(sf::Color(180, 214, 255));
        help.setPosition({70.f, 580.f});
        window.draw(help);
    }

    void drawOptions() {
        sf::Text title;
        title.setFont(font);
        title.setString("Opzioni grafiche");
        title.setCharacterSize(40);
        title.setFillColor(sf::Color(228, 188, 95));
        title.setPosition({440.f, 40.f});
        window.draw(title);

        for (const auto& op : optionButtons) {
            window.draw(op.box);
            window.draw(op.label);
        }

        sf::Text text;
        text.setFont(font);
        text.setString("Seleziona la risoluzione e premi Invio per confermare.");
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(230, 220, 196));
        text.setPosition({345.f, 615.f});
        window.draw(text);
    }

    void drawCharacterCreation() {
        sf::Text title;
        title.setFont(font);
        title.setString("Creazione del personaggio");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color(228, 188, 95));
        title.setPosition({370.f, 30.f});
        window.draw(title);

        sf::Text nameLabel;
        nameLabel.setFont(font);
        nameLabel.setString("Nome del protagonista");
        nameLabel.setCharacterSize(22);
        nameLabel.setPosition({120.f, 90.f});
        nameLabel.setFillColor(sf::Color(233, 225, 190));
        window.draw(nameLabel);

        sf::RectangleShape inputBox({520.f, 40.f});
        inputBox.setPosition({120.f, 125.f});
        inputBox.setFillColor(sf::Color(52, 42, 34));
        inputBox.setOutlineColor(sf::Color(228, 188, 95));
        inputBox.setOutlineThickness(2.f);
        window.draw(inputBox);

        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(playerInputName.empty() ? "Lorenzo" : playerInputName);
        nameText.setCharacterSize(22);
        nameText.setPosition({140.f, 133.f});
        nameText.setFillColor(sf::Color::White);
        window.draw(nameText);

        sf::Text pointsText;
        pointsText.setFont(font);
        pointsText.setString("Punti disponibili: " + std::to_string(statPoints));
        pointsText.setCharacterSize(22);
        pointsText.setPosition({760.f, 90.f});
        pointsText.setFillColor(sf::Color(180, 214, 255));
        window.draw(pointsText);

        static const std::array<int, 11> statValues = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };
        (void)statValues;

        for (size_t i = 0; i < statLabels.size(); ++i) {
            sf::RectangleShape statBack({420.f, 28.f});
            statBack.setPosition({420.f, 176.f + static_cast<float>(i) * 36.f});
            statBack.setFillColor((i == selectedStatIndex) ? sf::Color(82, 69, 50) : sf::Color(56, 47, 39));
            statBack.setOutlineColor(sf::Color::Black);
            window.draw(statBack);

            auto getStatValue = [&](size_t idx) -> int {
                switch (idx) {
                    case 0: return player.stats.vitalita;
                    case 1: return player.stats.resistenza;
                    case 2: return player.stats.cucina;
                    case 3: return player.stats.fascino;
                    case 4: return player.stats.armiCorte;
                    case 5: return player.stats.armiLunghe;
                    case 6: return player.stats.aste;
                    case 7: return player.stats.mazze;
                    case 8: return player.stats.scudi;
                    case 9: return player.stats.archi;
                    case 10: return player.stats.balestre;
                    default: return 1;
                }
            };

            int value = getStatValue(i);
            sf::RectangleShape fill({std::min(300.f, value * 20.f), 24.f});
            fill.setPosition({statBack.getPosition().x + 2.f, statBack.getPosition().y + 2.f});
            fill.setFillColor(sf::Color(184, 148, 68));
            window.draw(fill);

            sf::Text text;
            text.setFont(font);
            text.setString(statLabels[i] + "   " + std::to_string(value));
            text.setCharacterSize(16);
            text.setPosition({statBack.getPosition().x + 12.f, statBack.getPosition().y + 2.f});
            text.setFillColor(sf::Color::White);
            window.draw(text);
        }

        sf::Text helper;
        helper.setFont(font);
        helper.setString("Freccette: cambia attributo. Destra/Sinistra: aumenta o riduci. Invio: conferma.");
        helper.setCharacterSize(18);
        helper.setFillColor(sf::Color(180, 220, 255));
        helper.setPosition({190.f, 650.f});
        window.draw(helper);
    }

    void drawWorld() {
        sf::RectangleShape sky({1280.f, 720.f});
        sky.setFillColor(sf::Color(26, 20, 18));
        window.draw(sky);

        float phase = std::fmod(worldTime, 24.f);
        float brightness = 0.4f + 0.6f * std::sin((phase / 24.f) * 3.14159f);
        sf::Color tint(40 + static_cast<int>(brightness * 30), 45 + static_cast<int>(brightness * 25), 38 + static_cast<int>(brightness * 20));
        sf::RectangleShape overlay({1280.f, 720.f});
        overlay.setFillColor(sf::Color(tint.r, tint.g, tint.b, 120));
        window.draw(overlay);

        for (int x = 0; x < 40; ++x) {
            for (int y = 0; y < 30; ++y) {
                sf::RectangleShape tile({24.f, 24.f});
                tile.setPosition({20.f + x * 24.f - camera.x, 12.f + y * 24.f - camera.y});
                tile.setFillColor((x + y) % 2 == 0 ? sf::Color(60, 80, 52) : sf::Color(52, 70, 44));
                window.draw(tile);
            }
        }

        for (const auto& npc : npcs) {
            sf::CircleShape circle(8.f);
            circle.setFillColor(npc.color);
            circle.setPosition({npc.position.x - camera.x, npc.position.y - camera.y});
            circle.setOrigin(8.f, 8.f);
            window.draw(circle);

            sf::Text name;
            name.setFont(font);
            name.setString(npc.name.substr(0, 9));
            name.setCharacterSize(12);
            name.setFillColor(sf::Color::White);
            name.setPosition({npc.position.x - camera.x + 10.f, npc.position.y - camera.y - 18.f});
            window.draw(name);
        }

        sf::CircleShape hero(12.f);
        hero.setFillColor(player.color);
        hero.setPosition({player.position.x - camera.x, player.position.y - camera.y});
        hero.setOrigin(12.f, 12.f);
        window.draw(hero);

        sf::Text hud;
        hud.setFont(font);
        std::ostringstream oss;
        oss << "Nome: " << player.name << "\nTitolo: " << player.title
            << "\nEtà: " << player.age << "\nHP: " << player.hp << "/" << player.maxHp
            << "\nFame: " << player.fame << "\nInfluenza: " << player.influence
            << "\nGiorno: " << static_cast<int>(worldTime / 24.f) + 1;
        hud.setString(oss.str());
        hud.setCharacterSize(18);
        hud.setFillColor(sf::Color::White);
        hud.setPosition({1020.f, 30.f});
        window.draw(hud);

        sf::Text socialRules;
        socialRules.setFont(font);
        socialRules.setString("Regole sociali 1223: matrimonio, figli, successione e tutela.\nIl figlio con almeno 14 anni può guidare e ereditare.");
        socialRules.setCharacterSize(16);
        socialRules.setFillColor(sf::Color(245, 210, 138));
        socialRules.setPosition({18.f, 650.f});
        window.draw(socialRules);
    }
};

}  // namespace

int main() {
    Game game;
    game.run();
    return 0;
}
