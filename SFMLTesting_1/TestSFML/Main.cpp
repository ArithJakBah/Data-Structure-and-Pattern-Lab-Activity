// Final-Fantasy-Tactics-style mini game (SFML 2.6, C++17)
//
// Build (Linux/macOS):
//   g++ -std=c++17 main.cpp -o tactics -lsfml-graphics -lsfml-window -lsfml-system
//
// Features: isometric height-map, CT (charge time) turn order, BFS movement with
// jump limits, basic attacks + one skill per class, AoE with friendly fire,
// height damage modifier, simple scoring AI, floating damage numbers.

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <random>
#include <string>
#include <vector>

namespace {

    constexpr int   MAP_W = 10, MAP_H = 10;
    constexpr float TW = 64.f, TH = 32.f, HSTEP = 14.f;   // tile width/height, pixels per height level
    constexpr float OX = 400.f, OY = 190.f;               // screen origin of the map
    constexpr int   WIN_W = 1100, WIN_H = 720;
    constexpr float PI = 3.14159265f;

    // Height map (rotationally symmetric so both sides are fair)
    const int HEIGHTS[MAP_H][MAP_W] = {
        {1, 1, 1, 0, 0, 0, 0, 1, 1, 1},
        {1, 2, 1, 0, 0, 0, 0, 1, 2, 1},
        {1, 1, 1, 0, 0, 0, 0, 1, 1, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 2, 2, 1, 0, 0, 0},
        {0, 0, 0, 1, 2, 2, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0, 0, 0, 1, 1, 1},
        {1, 2, 1, 0, 0, 0, 0, 1, 2, 1},
        {1, 1, 1, 0, 0, 0, 0, 1, 1, 1},
    };

    int  tileH(int x, int y) { return HEIGHTS[y][x]; }
    bool inMap(int x, int y) { return x >= 0 && y >= 0 && x < MAP_W && y < MAP_H; }
    int  idx(int x, int y) { return y * MAP_W + x; }
    int  manh(int ax, int ay, int bx, int by) { return std::abs(ax - bx) + std::abs(ay - by); }

    sf::Vector2f project(float x, float y, float h) {
        return { OX + (x - y) * TW / 2.f, OY + (x + y) * TH / 2.f - h * HSTEP };
    }

    enum class Team { Player, Enemy };
    enum class Cls { Knight, Archer, Mage, Cleric };

    struct Action {
        std::string name;
        int   mp;
        int   range;
        int   aoe;      // manhattan radius around the target tile
        float power;
        bool  heal;
        bool  magic;
    };

    struct Unit {
        std::string name;
        Team team = Team::Player;
        Cls  cls = Cls::Knight;
        int  x = 0, y = 0;
        float vx = 0, vy = 0, vz = 0;   // visual position (for animation)
        int  hp = 1, maxHp = 1, mp = 0, maxMp = 0;
        int  atk = 0, mag = 0, def = 0, move = 3, jump = 1, speed = 8, ct = 0;
        bool alive = true, moved = false, acted = false;
        float flash = 0.f;
        Action basic{ "Attack", 0, 1, 0, 1.f, false, false };
        Action skill{ "Skill", 0, 1, 0, 1.f, false, false };
    };

    std::string className(Cls c) {
        switch (c) {
        case Cls::Knight: return "Knight";
        case Cls::Archer: return "Archer";
        case Cls::Mage:   return "Mage";
        case Cls::Cleric: return "Cleric";
        }
        return "";
    }

    Unit makeUnit(Cls c, Team t, int x, int y, const std::string& name) {
        Unit u;
        u.name = name; u.team = t; u.cls = c;
        u.x = x; u.y = y; u.vx = (float)x; u.vy = (float)y; u.vz = (float)tileH(x, y);
        switch (c) {
        case Cls::Knight:
            u.maxHp = 48; u.maxMp = 8;  u.atk = 14; u.mag = 2;  u.def = 6; u.move = 4; u.jump = 1; u.speed = 8;
            u.basic = { "Attack", 0, 1, 0, 1.0f, false, false };
            u.skill = { "Power Strike", 4, 1, 0, 1.7f, false, false };
            break;
        case Cls::Archer:
            u.maxHp = 36; u.maxMp = 8;  u.atk = 11; u.mag = 3;  u.def = 3; u.move = 4; u.jump = 2; u.speed = 10;
            u.basic = { "Shoot", 0, 4, 0, 1.0f, false, false };
            u.skill = { "Aimed Shot", 4, 5, 0, 1.6f, false, false };
            break;
        case Cls::Mage:
            u.maxHp = 28; u.maxMp = 20; u.atk = 4;  u.mag = 14; u.def = 2; u.move = 3; u.jump = 1; u.speed = 9;
            u.basic = { "Staff", 0, 1, 0, 1.0f, false, false };
            u.skill = { "Fire", 6, 4, 1, 1.5f, false, true };
            break;
        case Cls::Cleric:
            u.maxHp = 32; u.maxMp = 20; u.atk = 5;  u.mag = 11; u.def = 3; u.move = 3; u.jump = 1; u.speed = 9;
            u.basic = { "Mace", 0, 1, 0, 1.0f, false, false };
            u.skill = { "Cure", 5, 3, 0, 1.8f, true, true };
            break;
        }
        u.hp = u.maxHp; u.mp = u.maxMp;
        return u;
    }

    struct FloatText { std::string s; sf::Vector2f pos; float t = 0; sf::Color c; };

    enum HL { HL_MOVE = 1, HL_RANGE = 2, HL_HEALRANGE = 4, HL_AOE = 8 };

    class Game {
    public:
        Game()
            : window(sf::VideoMode(WIN_W, WIN_H), "Tactics", sf::Style::Titlebar | sf::Style::Close) {
            window.setFramerateLimit(60);
            loadFont();
            reset();
        }

        void run() {
            sf::Clock clock;
            while (window.isOpen()) {
                sf::Event ev;
                while (window.pollEvent(ev)) handleEvent(ev);
                float dt = std::min(clock.restart().asSeconds(), 0.05f);
                update(dt);
                draw();
            }
        }

    private:
        enum class Mode { Menu, SelectMove, Moving, SelectTarget, Acting, AIThink, Over };
        struct Plan { int mx, my, action, tx, ty; };

        sf::RenderWindow window;
        sf::Font font;
        bool fontOk = false;

        std::vector<Unit> units;
        int  active = 0;
        Mode mode = Mode::Menu;
        int  pendingAction = 0;   // 0 = basic, 1 = skill
        int  winner = 0;          // 1 = player, 2 = enemy

        std::vector<int> reachDist;
        std::vector<sf::Vector2i> reachPrev;
        std::vector<sf::Vector2i> path;
        size_t pathIdx = 0;
        float  pathT = 0.f;
        float  timer = 0.f;
        Plan   plan{ 0, 0, -1, 0, 0 };

        sf::Vector2i hover{ -1, -1 };
        sf::Vector2f mousePos{ 0, 0 };
        std::vector<std::string> logs;
        std::vector<FloatText> floats;
        std::mt19937 rng{ std::random_device{}() };
        float time = 0.f;

        // ------------------------------------------------------------ setup
        void loadFont() {
            const char* paths[] = {
                "font.ttf", "assets/font.ttf",
                "C:/Windows/Fonts/arial.ttf",
                "/System/Library/Fonts/Supplemental/Arial.ttf",
                "/Library/Fonts/Arial.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/TTF/DejaVuSans.ttf",
                "/usr/share/fonts/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            };
            for (auto p : paths) {
                if (font.loadFromFile(p)) { fontOk = true; return; }
            }
        }

        void reset() {
            units.clear(); logs.clear(); floats.clear();
            units.push_back(makeUnit(Cls::Knight, Team::Player, 3, 7, "Aldric"));
            units.push_back(makeUnit(Cls::Archer, Team::Player, 1, 8, "Lyra"));
            units.push_back(makeUnit(Cls::Mage, Team::Player, 2, 9, "Merlo"));
            units.push_back(makeUnit(Cls::Cleric, Team::Player, 0, 8, "Sera"));
            units.push_back(makeUnit(Cls::Knight, Team::Enemy, 6, 2, "Brute"));
            units.push_back(makeUnit(Cls::Archer, Team::Enemy, 8, 1, "Bandit"));
            units.push_back(makeUnit(Cls::Mage, Team::Enemy, 7, 0, "Hexer"));
            units.push_back(makeUnit(Cls::Cleric, Team::Enemy, 9, 1, "Shaman"));
            for (auto& u : units) u.ct = (int)(rng() % 30);
            winner = 0;
            addLog("Defeat all enemies!");
            beginTurn();
        }

        void addLog(const std::string& s) {
            logs.push_back(s);
            if (logs.size() > 6) logs.erase(logs.begin());
        }

        // ------------------------------------------------------------ queries
        Unit* unitAt(int x, int y) {
            for (auto& u : units)
                if (u.alive && u.x == x && u.y == y) return &u;
            return nullptr;
        }

        // Unit standing on (x,y) assuming `u` is standing on T instead of its real tile.
        Unit* unitAtFrom(Unit& u, sf::Vector2i T, int x, int y) {
            if (x == T.x && y == T.y) return &u;
            if (x == u.x && y == u.y) return nullptr;
            return unitAt(x, y);
        }

        const Action& curAction() { return pendingAction == 0 ? units[active].basic : units[active].skill; }

        bool validTarget(Unit& u, const Action& ac, sf::Vector2i T, int tx, int ty) {
            if (!inMap(tx, ty)) return false;
            if (manh(T.x, T.y, tx, ty) > ac.range) return false;
            if (ac.aoe > 0) return true;
            Unit* t = unitAtFrom(u, T, tx, ty);
            if (!t) return false;
            if (ac.heal) return t->team == u.team;
            return t->team != u.team;
        }

        int calcAmount(const Unit& a, int ah, const Unit& d, int dh, const Action& ac, bool randomize) {
            float v;
            if (ac.heal) {
                v = a.mag * ac.power;
            }
            else {
                float base = (ac.magic ? a.mag : a.atk) * ac.power;
                float def = ac.magic ? d.def * 0.4f : (float)d.def;
                v = base - def;
                if (!ac.magic) {
                    int diff = std::clamp(ah - dh, -3, 3);
                    v *= 1.f + 0.1f * diff;   // high ground bonus / low ground penalty
                }
            }
            if (randomize) {
                std::uniform_real_distribution<float> r(0.9f, 1.1f);
                v *= r(rng);
            }
            return std::max(1, (int)std::lround(v));
        }

        // ------------------------------------------------------------ movement
        void computeReach(const Unit& u, std::vector<int>& dist, std::vector<sf::Vector2i>& prev) {
            dist.assign(MAP_W * MAP_H, -1);
            prev.assign(MAP_W * MAP_H, sf::Vector2i(-1, -1));
            std::queue<sf::Vector2i> q;
            dist[idx(u.x, u.y)] = 0;
            q.push({ u.x, u.y });
            const int dx[4] = { 1, -1, 0, 0 }, dy[4] = { 0, 0, 1, -1 };
            while (!q.empty()) {
                sf::Vector2i c = q.front(); q.pop();
                int d = dist[idx(c.x, c.y)];
                if (d >= u.move) continue;
                for (int k = 0; k < 4; ++k) {
                    int nx = c.x + dx[k], ny = c.y + dy[k];
                    if (!inMap(nx, ny) || dist[idx(nx, ny)] != -1) continue;
                    if (std::abs(tileH(nx, ny) - tileH(c.x, c.y)) > u.jump) continue;
                    Unit* o = unitAt(nx, ny);
                    if (o && o->team != u.team) continue;   // can't walk through enemies
                    dist[idx(nx, ny)] = d + 1;
                    prev[idx(nx, ny)] = c;
                    q.push({ nx, ny });
                }
            }
        }

        bool canStop(const Unit& u, int x, int y) {
            Unit* o = unitAt(x, y);
            return !o || o == &u;
        }

        bool isReachable(const Unit& u, int x, int y) {
            return inMap(x, y) && reachDist[idx(x, y)] >= 0 && canStop(u, x, y);
        }

        std::vector<sf::Vector2i> buildPath(int tx, int ty) {
            std::vector<sf::Vector2i> p;
            sf::Vector2i c(tx, ty);
            while (c.x != -1) {
                p.push_back(c);
                c = reachPrev[idx(c.x, c.y)];
            }
            std::reverse(p.begin(), p.end());
            return p;
        }

        void startMove(const std::vector<sf::Vector2i>& p) {
            if (p.size() < 2) { onMoveDone(); return; }
            path = p; pathIdx = 0; pathT = 0.f;
            mode = Mode::Moving;
        }

        void onMoveDone() {
            Unit& u = units[active];
            if (u.team == Team::Enemy) {
                aiAct();
            }
            else {
                if (u.moved && u.acted) endTurn();
                else mode = Mode::Menu;
            }
        }

        // ------------------------------------------------------------ turns
        void advanceToNext() {
            for (int guard = 0; guard < 10000; ++guard) {
                int best = -1;
                for (int i = 0; i < (int)units.size(); ++i)
                    if (units[i].alive && units[i].ct >= 100 && (best < 0 || units[i].ct > units[best].ct)) best = i;
                if (best >= 0) { active = best; return; }
                for (auto& u : units) if (u.alive) u.ct += u.speed;
            }
        }

        void beginTurn() {
            advanceToNext();
            Unit& u = units[active];
            u.moved = u.acted = false;
            u.mp = std::min(u.maxMp, u.mp + 2);
            addLog("-- " + u.name + "'s turn --");
            if (u.team == Team::Enemy) { mode = Mode::AIThink; timer = 0.6f; }
            else mode = Mode::Menu;
        }

        bool checkOver() {
            int p = 0, e = 0;
            for (auto& u : units) if (u.alive) (u.team == Team::Player ? p : e)++;
            if (p == 0) { winner = 2; mode = Mode::Over; return true; }
            if (e == 0) { winner = 1; mode = Mode::Over; return true; }
            return false;
        }

        void endTurn() {
            Unit& u = units[active];
            u.ct -= 60 + (u.moved ? 20 : 0) + (u.acted ? 20 : 0);
            if (checkOver()) return;
            beginTurn();
        }

        std::vector<int> predictOrder(int n) {
            std::vector<int> ct(units.size());
            for (size_t i = 0; i < units.size(); ++i) ct[i] = units[i].ct;
            std::vector<int> order;
            for (int k = 0; k < n; ++k) {
                for (int guard = 0;; ++guard) {
                    if (guard > 1000) return order;
                    int best = -1;
                    for (int i = 0; i < (int)units.size(); ++i)
                        if (units[i].alive && ct[i] >= 100 && (best < 0 || ct[i] > ct[best])) best = i;
                    if (best >= 0) { order.push_back(best); ct[best] -= 60; break; }
                    for (size_t i = 0; i < units.size(); ++i) if (units[i].alive) ct[i] += units[i].speed;
                }
            }
            return order;
        }

        // ------------------------------------------------------------ actions
        void spawnFloat(const Unit& t, const std::string& s, sf::Color c) {
            floats.push_back({ s, project(t.vx, t.vy, t.vz) + sf::Vector2f(0, -62), 0.f, c });
        }

        void doAction(Unit& a, const Action& ac, int tx, int ty) {
            a.mp -= ac.mp;
            a.acted = true;
            addLog(a.name + " uses " + ac.name + "!");
            for (auto& t : units) {
                if (!t.alive) continue;
                if (manh(t.x, t.y, tx, ty) > ac.aoe) continue;
                if (ac.heal) {
                    if (t.team != a.team) continue;
                    int amt = calcAmount(a, tileH(a.x, a.y), t, tileH(t.x, t.y), ac, true);
                    amt = std::min(amt, t.maxHp - t.hp);
                    t.hp += amt;
                    spawnFloat(t, "+" + std::to_string(amt), sf::Color(110, 255, 140));
                    addLog(t.name + " recovers " + std::to_string(amt) + " HP.");
                }
                else {
                    if (ac.aoe == 0 && &t == &a) continue;
                    int amt = calcAmount(a, tileH(a.x, a.y), t, tileH(t.x, t.y), ac, true);
                    t.hp -= amt;
                    t.flash = 0.3f;
                    spawnFloat(t, "-" + std::to_string(amt), sf::Color(255, 110, 90));
                    addLog(t.name + " takes " + std::to_string(amt) + " damage.");
                    if (t.hp <= 0) {
                        t.hp = 0; t.alive = false;
                        addLog(t.name + " is defeated!");
                    }
                }
            }
            mode = Mode::Acting;
            timer = 0.8f;
        }

        // ------------------------------------------------------------ AI
        float scoreAction(Unit& u, sf::Vector2i T, const Action& ac, int tx, int ty) {
            if (!validTarget(u, ac, T, tx, ty)) return 0.f;
            float s = 0.f;
            for (auto& t : units) {
                if (!t.alive) continue;
                sf::Vector2i tp = (&t == &u) ? T : sf::Vector2i(t.x, t.y);
                if (manh(tp.x, tp.y, tx, ty) > ac.aoe) continue;
                int amt = calcAmount(u, tileH(T.x, T.y), t, tileH(tp.x, tp.y), ac, false);
                if (ac.heal) {
                    if (t.team != u.team) continue;
                    if (t.hp > t.maxHp * 0.6f) continue;
                    s += (float)std::min(amt, t.maxHp - t.hp);
                }
                else {
                    if (&t == &u && ac.aoe == 0) continue;
                    if (t.team != u.team) {
                        s += amt;
                        if (amt >= t.hp) s += 25.f;
                    }
                    else {
                        s -= amt * 1.5f;   // friendly fire is bad
                    }
                }
            }
            return s;
        }

        void aiThink() {
            Unit& u = units[active];
            computeReach(u, reachDist, reachPrev);
            std::vector<sf::Vector2i> tiles;
            for (int y = 0; y < MAP_H; ++y)
                for (int x = 0; x < MAP_W; ++x)
                    if (isReachable(u, x, y)) tiles.push_back({ x, y });

            std::uniform_real_distribution<float> noise(0.f, 0.5f);
            float bestScore = 0.f;
            Plan best{ u.x, u.y, -1, 0, 0 };
            bool found = false;
            for (auto T : tiles) {
                for (int a = 0; a < 2; ++a) {
                    const Action& ac = a == 0 ? u.basic : u.skill;
                    if (ac.mp > u.mp) continue;
                    for (int ty = 0; ty < MAP_H; ++ty)
                        for (int tx = 0; tx < MAP_W; ++tx) {
                            if (manh(T.x, T.y, tx, ty) > ac.range) continue;
                            float s = scoreAction(u, T, ac, tx, ty);
                            if (s <= 0.f) continue;
                            s += 0.15f * tileH(T.x, T.y) + noise(rng);
                            if (!found || s > bestScore) {
                                found = true; bestScore = s;
                                best = { T.x, T.y, a, tx, ty };
                            }
                        }
                }
            }
            if (!found) {
                // No attack available: approach the nearest player unit.
                int bestD = 1 << 20;
                for (auto T : tiles) {
                    int d = 1 << 20;
                    for (auto& o : units)
                        if (o.alive && o.team != u.team) d = std::min(d, manh(T.x, T.y, o.x, o.y));
                    d = d * 4 - tileH(T.x, T.y);
                    if (d < bestD) { bestD = d; best = { T.x, T.y, -1, 0, 0 }; }
                }
            }
            plan = best;
            if (plan.mx != u.x || plan.my != u.y) startMove(buildPath(plan.mx, plan.my));
            else onMoveDone();
        }

        void aiAct() {
            Unit& u = units[active];
            if (plan.action >= 0) {
                const Action& ac = plan.action == 0 ? u.basic : u.skill;
                doAction(u, ac, plan.tx, plan.ty);
            }
            else {
                mode = Mode::Acting;
                timer = 0.3f;
            }
        }

        // ------------------------------------------------------------ input
        struct Button { sf::FloatRect r; };
        sf::FloatRect buttonRect(int i) { return sf::FloatRect(800.f, 450.f + i * 46.f, 270.f, 38.f); }

        bool buttonsActive() {
            return units[active].team == Team::Player &&
                (mode == Mode::Menu || mode == Mode::SelectMove || mode == Mode::SelectTarget);
        }

        bool buttonEnabled(int b) {
            Unit& u = units[active];
            switch (b) {
            case 0: return !u.moved;
            case 1: return !u.acted;
            case 2: return !u.acted && u.mp >= u.skill.mp;
            default: return true;
            }
        }

        int buttonAt(sf::Vector2f p) {
            if (!buttonsActive()) return -1;
            for (int i = 0; i < 4; ++i) if (buttonRect(i).contains(p)) return i;
            return -1;
        }

        void pressButton(int b) {
            if (!buttonEnabled(b)) return;
            Unit& u = units[active];
            switch (b) {
            case 0: computeReach(u, reachDist, reachPrev); mode = Mode::SelectMove; break;
            case 1: pendingAction = 0; mode = Mode::SelectTarget; break;
            case 2: pendingAction = 1; mode = Mode::SelectTarget; break;
            case 3: endTurn(); break;
            }
        }

        void handleEvent(const sf::Event& ev) {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::R) { reset(); return; }
                if (ev.key.code == sf::Keyboard::Escape && buttonsActive()) mode = Mode::Menu;
                if (buttonsActive()) {
                    if (ev.key.code == sf::Keyboard::M) pressButton(0);
                    if (ev.key.code == sf::Keyboard::A) pressButton(1);
                    if (ev.key.code == sf::Keyboard::S) pressButton(2);
                    if (ev.key.code == sf::Keyboard::W) pressButton(3);
                }
            }
            if (ev.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mp = window.mapPixelToCoords({ ev.mouseButton.x, ev.mouseButton.y });
                if (ev.mouseButton.button == sf::Mouse::Right) {
                    if (buttonsActive()) mode = Mode::Menu;
                }
                else if (ev.mouseButton.button == sf::Mouse::Left) {
                    onLeftClick(mp);
                }
            }
        }

        void onLeftClick(sf::Vector2f mp) {
            if (mode == Mode::Over || units[active].team != Team::Player) return;
            int b = buttonAt(mp);
            if (b >= 0) { pressButton(b); return; }
            Unit& u = units[active];
            if (mode == Mode::SelectMove) {
                if (isReachable(u, hover.x, hover.y)) startMove(buildPath(hover.x, hover.y));
            }
            else if (mode == Mode::SelectTarget) {
                const Action& ac = curAction();
                if (validTarget(u, ac, { u.x, u.y }, hover.x, hover.y)) doAction(u, ac, hover.x, hover.y);
            }
        }

        sf::Vector2i pickTile(sf::Vector2f p) {
            // Units first (their bodies stick up above the tile)
            const Unit* bestU = nullptr;
            for (auto& u : units) {
                if (!u.alive) continue;
                sf::Vector2f f = project(u.vx, u.vy, u.vz);
                if (p.x > f.x - 13 && p.x < f.x + 13 && p.y > f.y - 46 && p.y < f.y + 4)
                    if (!bestU || u.vx + u.vy > bestU->vx + bestU->vy) bestU = &u;
            }
            if (bestU) return { bestU->x, bestU->y };
            for (int y = MAP_H - 1; y >= 0; --y)
                for (int x = MAP_W - 1; x >= 0; --x) {
                    sf::Vector2f c = project((float)x, (float)y, (float)tileH(x, y));
                    if (std::abs(p.x - c.x) / (TW / 2) + std::abs(p.y - c.y) / (TH / 2) <= 1.f) return { x, y };
                }
            return { -1, -1 };
        }

        // ------------------------------------------------------------ update
        void update(float dt) {
            time += dt;
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            hover = pickTile(mousePos);

            for (auto& u : units) if (u.flash > 0) u.flash -= dt;
            for (auto& f : floats) { f.t += dt; f.pos.y -= 28.f * dt; }
            floats.erase(std::remove_if(floats.begin(), floats.end(), [](const FloatText& f) { return f.t > 1.3f; }),
                floats.end());

            switch (mode) {
            case Mode::AIThink:
                timer -= dt;
                if (timer <= 0) aiThink();
                break;
            case Mode::Moving: updateMoving(dt); break;
            case Mode::Acting:
                timer -= dt;
                if (timer <= 0) {
                    if (checkOver()) break;
                    Unit& u = units[active];
                    if (u.team == Team::Enemy || (u.moved && u.acted)) endTurn();
                    else mode = Mode::Menu;
                }
                break;
            default: break;
            }
        }

        void updateMoving(float dt) {
            Unit& u = units[active];
            pathT += dt * 5.f;
            while (pathT >= 1.f && pathIdx + 1 < path.size()) {
                pathT -= 1.f;
                ++pathIdx;
            }
            if (pathIdx + 1 >= path.size()) {
                sf::Vector2i e = path.back();
                u.x = e.x; u.y = e.y;
                u.vx = (float)e.x; u.vy = (float)e.y; u.vz = (float)tileH(e.x, e.y);
                u.moved = true;
                onMoveDone();
                return;
            }
            sf::Vector2i a = path[pathIdx], b = path[pathIdx + 1];
            float t = pathT;
            u.vx = a.x + (b.x - a.x) * t;
            u.vy = a.y + (b.y - a.y) * t;
            u.vz = tileH(a.x, a.y) + (tileH(b.x, b.y) - tileH(a.x, a.y)) * t + std::sin(PI * t) * 0.3f;
            // keep logical position in sync with the nearest tile for draw ordering
            u.x = (int)std::lround(u.vx); u.y = (int)std::lround(u.vy);
        }

        // ------------------------------------------------------------ drawing helpers
        void drawText(const std::string& s, float x, float y, unsigned size = 16,
            sf::Color c = sf::Color::White, bool center = false) {
            if (!fontOk) return;
            sf::Text t(s, font, size);
            t.setFillColor(c);
            t.setOutlineColor(sf::Color(0, 0, 0, 200));
            t.setOutlineThickness(1.f);
            if (center) {
                sf::FloatRect b = t.getLocalBounds();
                t.setOrigin(b.left + b.width / 2.f, 0);
            }
            t.setPosition(x, y);
            window.draw(t);
        }

        void diamond(sf::Vector2f c, float k, sf::Color fill, sf::Color outline = sf::Color::Transparent, float ot = 0.f) {
            sf::ConvexShape s(4);
            s.setPoint(0, { c.x, c.y - TH / 2 * k });
            s.setPoint(1, { c.x + TW / 2 * k, c.y });
            s.setPoint(2, { c.x, c.y + TH / 2 * k });
            s.setPoint(3, { c.x - TW / 2 * k, c.y });
            s.setFillColor(fill);
            s.setOutlineColor(outline);
            s.setOutlineThickness(ot);
            window.draw(s);
        }

        void bar(float x, float y, float w, float h, float frac, sf::Color col) {
            sf::RectangleShape bg({ w, h });
            bg.setPosition(x, y); bg.setFillColor(sf::Color(15, 15, 20, 230));
            bg.setOutlineColor(sf::Color(0, 0, 0)); bg.setOutlineThickness(1.f);
            window.draw(bg);
            sf::RectangleShape fg({ w * std::clamp(frac, 0.f, 1.f), h });
            fg.setPosition(x, y); fg.setFillColor(col);
            window.draw(fg);
        }

        sf::Color classColor(Cls c) {
            switch (c) {
            case Cls::Knight: return { 170, 175, 190 };
            case Cls::Archer: return { 70, 150, 70 };
            case Cls::Mage:   return { 140, 80, 190 };
            case Cls::Cleric: return { 235, 235, 235 };
            }
            return sf::Color::White;
        }

        void drawTile(int x, int y, const std::vector<int>& hl) {
            int h = tileH(x, y);
            sf::Vector2f c = project((float)x, (float)y, (float)h);
            float d = (h + 1) * HSTEP;

            sf::ConvexShape L(4), R(4);
            L.setPoint(0, { c.x - TW / 2, c.y });     L.setPoint(1, { c.x, c.y + TH / 2 });
            L.setPoint(2, { c.x, c.y + TH / 2 + d }); L.setPoint(3, { c.x - TW / 2, c.y + d });
            R.setPoint(0, { c.x + TW / 2, c.y });     R.setPoint(1, { c.x, c.y + TH / 2 });
            R.setPoint(2, { c.x, c.y + TH / 2 + d }); R.setPoint(3, { c.x + TW / 2, c.y + d });
            L.setFillColor({ 112, 82, 56 }); R.setFillColor({ 88, 64, 44 });
            window.draw(L); window.draw(R);

            int shade = ((x + y) & 1) ? 10 : 0;
            sf::Color top(90 + h * 28 + shade, 165 + h * 14 + shade, 80 + h * 22 + shade);
            diamond(c, 1.f, top, sf::Color(0, 0, 0, 55), 1.f);

            int f = hl[idx(x, y)];
            if (f & HL_MOVE)      diamond(c, 0.92f, sf::Color(60, 120, 255, 140));
            if (f & HL_RANGE)     diamond(c, 0.92f, sf::Color(255, 70, 70, 120));
            if (f & HL_HEALRANGE) diamond(c, 0.92f, sf::Color(70, 220, 120, 120));
            if (f & HL_AOE)       diamond(c, 0.80f, sf::Color(255, 170, 40, 190));
            if (x == hover.x && y == hover.y) diamond(c, 0.98f, sf::Color::Transparent, sf::Color(255, 240, 80), 2.f);

            for (auto& u : units) {
                if (!u.alive) continue;
                if ((int)std::lround(u.vx) == x && (int)std::lround(u.vy) == y) drawUnit(u);
            }
        }

        void drawUnit(const Unit& u) {
            sf::Vector2f p = project(u.vx, u.vy, u.vz);
            sf::Color teamCol = u.team == Team::Player ? sf::Color(70, 140, 255) : sf::Color(235, 70, 70);
            float fl = std::clamp(u.flash / 0.3f, 0.f, 1.f);
            auto mix = [&](sf::Color c) {
                return sf::Color((sf::Uint8)(c.r + (255 - c.r) * fl), (sf::Uint8)(c.g + (255 - c.g) * fl),
                    (sf::Uint8)(c.b + (255 - c.b) * fl));
                };

            sf::CircleShape sh(14.f);
            sh.setOrigin(14, 14); sh.setScale(1.f, 0.5f); sh.setPosition(p.x, p.y + 1);
            sh.setFillColor(sf::Color(0, 0, 0, 90));
            window.draw(sh);

            sf::CircleShape ring(15.f);
            ring.setOrigin(15, 15); ring.setScale(1.f, 0.5f); ring.setPosition(p.x, p.y);
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(teamCol); ring.setOutlineThickness(3.f);
            window.draw(ring);

            sf::RectangleShape body({ 18, 24 });
            body.setOrigin(9, 24); body.setPosition(p.x, p.y - 2);
            body.setFillColor(mix(classColor(u.cls)));
            body.setOutlineColor(sf::Color(0, 0, 0, 160)); body.setOutlineThickness(1.f);
            window.draw(body);

            sf::CircleShape head(8.f);
            head.setOrigin(8, 8); head.setPosition(p.x, p.y - 33);
            head.setFillColor(mix(sf::Color(240, 205, 165)));
            head.setOutlineColor(sf::Color(0, 0, 0, 160)); head.setOutlineThickness(1.f);
            window.draw(head);

            switch (u.cls) {
            case Cls::Knight: {
                sf::RectangleShape helm({ 18, 7 });
                helm.setOrigin(9, 7); helm.setPosition(p.x, p.y - 34);
                helm.setFillColor(sf::Color(120, 125, 140));
                window.draw(helm);
                sf::RectangleShape shield({ 7, 13 });
                shield.setPosition(p.x + 8, p.y - 22);
                shield.setFillColor(teamCol);
                shield.setOutlineColor(sf::Color::Black); shield.setOutlineThickness(1.f);
                window.draw(shield);
                break;
            }
            case Cls::Archer: {
                sf::RectangleShape bow({ 3, 24 });
                bow.setPosition(p.x + 10, p.y - 28);
                bow.setFillColor(sf::Color(130, 85, 40));
                window.draw(bow);
                break;
            }
            case Cls::Mage: {
                sf::ConvexShape hat(3);
                hat.setPoint(0, { p.x - 10, p.y - 38 });
                hat.setPoint(1, { p.x + 10, p.y - 38 });
                hat.setPoint(2, { p.x + 2, p.y - 56 });
                hat.setFillColor(sf::Color(90, 50, 150));
                hat.setOutlineColor(sf::Color::Black); hat.setOutlineThickness(1.f);
                window.draw(hat);
                break;
            }
            case Cls::Cleric: {
                sf::RectangleShape v({ 4, 12 }), hz({ 12, 4 });
                v.setOrigin(2, 6); v.setPosition(p.x, p.y - 14);
                hz.setOrigin(6, 2); hz.setPosition(p.x, p.y - 14);
                v.setFillColor(sf::Color(210, 50, 50)); hz.setFillColor(sf::Color(210, 50, 50));
                window.draw(v); window.draw(hz);
                break;
            }
            }

            bar(p.x - 16, p.y - 60, 32, 5, (float)u.hp / u.maxHp,
                u.team == Team::Player ? sf::Color(80, 220, 110) : sf::Color(230, 80, 70));

            if (&u == &units[active] && mode != Mode::Over) {
                float bob = std::sin(time * 6.f) * 3.f;
                sf::ConvexShape tri(3);
                tri.setPoint(0, { p.x - 6, p.y - 74 + bob });
                tri.setPoint(1, { p.x + 6, p.y - 74 + bob });
                tri.setPoint(2, { p.x, p.y - 64 + bob });
                tri.setFillColor(sf::Color(255, 230, 70));
                tri.setOutlineColor(sf::Color::Black); tri.setOutlineThickness(1.f);
                window.draw(tri);
            }
        }

        std::vector<int> computeHighlights() {
            std::vector<int> hl(MAP_W * MAP_H, 0);
            if (units[active].team != Team::Player) return hl;
            Unit& u = units[active];
            if (mode == Mode::SelectMove) {
                for (int y = 0; y < MAP_H; ++y)
                    for (int x = 0; x < MAP_W; ++x)
                        if (isReachable(u, x, y)) hl[idx(x, y)] |= HL_MOVE;
            }
            else if (mode == Mode::SelectTarget) {
                const Action& ac = curAction();
                for (int y = 0; y < MAP_H; ++y)
                    for (int x = 0; x < MAP_W; ++x)
                        if (manh(u.x, u.y, x, y) <= ac.range) hl[idx(x, y)] |= ac.heal ? HL_HEALRANGE : HL_RANGE;
                if (validTarget(u, ac, { u.x, u.y }, hover.x, hover.y))
                    for (int y = 0; y < MAP_H; ++y)
                        for (int x = 0; x < MAP_W; ++x)
                            if (manh(hover.x, hover.y, x, y) <= ac.aoe) hl[idx(x, y)] |= HL_AOE;
            }
            return hl;
        }

        void drawHUD() {
            sf::RectangleShape panel({ 320.f, (float)WIN_H });
            panel.setPosition(780, 0);
            panel.setFillColor(sf::Color(20, 22, 30, 240));
            window.draw(panel);

            // Turn order
            drawText("TURN ORDER", 800, 12, 14, sf::Color(150, 160, 180));
            auto order = predictOrder(8);
            for (size_t i = 0; i < order.size(); ++i) {
                const Unit& u = units[order[i]];
                float y = 34.f + i * 22.f;
                sf::RectangleShape sq({ 12, 12 });
                sq.setPosition(802, y + 3);
                sq.setFillColor(u.team == Team::Player ? sf::Color(70, 140, 255) : sf::Color(235, 70, 70));
                window.draw(sq);
                drawText(u.name + "  (" + className(u.cls) + ")", 822, y, 15,
                    i == 0 ? sf::Color(255, 230, 90) : sf::Color(220, 220, 225));
            }

            // Unit info (hovered unit, else active unit)
            const Unit* iu = &units[active];
            if (hover.x >= 0) if (Unit* h = unitAt(hover.x, hover.y)) iu = h;
            float y0 = 225.f;
            drawText("UNIT", 800, y0, 14, sf::Color(150, 160, 180));
            drawText(iu->name + " - " + className(iu->cls), 800, y0 + 22, 18,
                iu->team == Team::Player ? sf::Color(130, 180, 255) : sf::Color(255, 130, 120));
            bar(800, y0 + 52, 200, 12, (float)iu->hp / iu->maxHp, sf::Color(80, 210, 110));
            drawText("HP " + std::to_string(iu->hp) + "/" + std::to_string(iu->maxHp), 1010, y0 + 48, 14);
            bar(800, y0 + 72, 200, 12, iu->maxMp ? (float)iu->mp / iu->maxMp : 0.f, sf::Color(80, 140, 235));
            drawText("MP " + std::to_string(iu->mp) + "/" + std::to_string(iu->maxMp), 1010, y0 + 68, 14);
            drawText("ATK " + std::to_string(iu->atk) + "   MAG " + std::to_string(iu->mag) + "   DEF " + std::to_string(iu->def),
                800, y0 + 94, 14);
            drawText("MOVE " + std::to_string(iu->move) + "   JUMP " + std::to_string(iu->jump) +
                "   SPD " + std::to_string(iu->speed) + "   CT " + std::to_string(iu->ct),
                800, y0 + 114, 14);
            drawText("Skill: " + iu->skill.name + " (" + std::to_string(iu->skill.mp) + " MP)", 800, y0 + 136, 14,
                sf::Color(200, 200, 140));
            if (hover.x >= 0)
                drawText("Tile height: " + std::to_string(tileH(hover.x, hover.y)), 800, y0 + 158, 14,
                    sf::Color(160, 170, 150));

            // Menu
            const Unit& au = units[active];
            if (au.team == Team::Player && mode != Mode::Over) {
                drawText(au.name + "'s turn", 800, 418, 17, sf::Color(255, 230, 90));
                const char* hot[4] = { "[M] Move", "[A] Attack", "[S] ", "[W] Wait" };
                for (int i = 0; i < 4; ++i) {
                    bool en = buttonEnabled(i);
                    bool sel = (i == 0 && mode == Mode::SelectMove) ||
                        (i == 1 && mode == Mode::SelectTarget && pendingAction == 0) ||
                        (i == 2 && mode == Mode::SelectTarget && pendingAction == 1);
                    sf::FloatRect r = buttonRect(i);
                    sf::RectangleShape b({ r.width, r.height });
                    b.setPosition(r.left, r.top);
                    bool over = r.contains(mousePos) && buttonsActive();
                    sf::Color fill = !en ? sf::Color(40, 42, 50) : sel ? sf::Color(80, 110, 190)
                        : over ? sf::Color(70, 78, 100) : sf::Color(52, 58, 76);
                    b.setFillColor(fill);
                    b.setOutlineColor(sel ? sf::Color(255, 230, 90) : sf::Color(90, 96, 120));
                    b.setOutlineThickness(1.5f);
                    window.draw(b);
                    std::string label = hot[i];
                    if (i == 2) label += au.skill.name + " (" + std::to_string(au.skill.mp) + " MP)";
                    drawText(label, r.left + 12, r.top + 8, 16, en ? sf::Color::White : sf::Color(110, 110, 120));
                }
            }
            else if (mode != Mode::Over) {
                drawText("Enemy turn...", 800, 440, 18, sf::Color(255, 130, 120));
            }
        }

        std::string hintText() {
            Unit& u = units[active];
            if (u.team != Team::Player) return "";
            switch (mode) {
            case Mode::Menu: return "Choose an action.  Right-click / Esc cancels a selection.";
            case Mode::SelectMove: return "Click a blue tile to move (limited by Move and Jump).";
            case Mode::SelectTarget: {
                const Action& ac = curAction();
                if (validTarget(u, ac, { u.x, u.y }, hover.x, hover.y)) {
                    std::string s = ac.name + ":";
                    for (auto& t : units) {
                        if (!t.alive || manh(t.x, t.y, hover.x, hover.y) > ac.aoe) continue;
                        if (ac.heal && t.team != u.team) continue;
                        if (!ac.heal && ac.aoe == 0 && &t == &u) continue;
                        int amt = calcAmount(u, tileH(u.x, u.y), t, tileH(t.x, t.y), ac, false);
                        s += "  " + t.name + (ac.heal ? " +" : " -") + std::to_string(amt);
                    }
                    return s + "   (click to confirm)";
                }
                return ac.aoe > 0 ? "Pick a tile in range (area attack - careful of friendly fire!)"
                    : "Pick a target in range.";
            }
            default: return "";
            }
        }

        void draw() {
            window.clear(sf::Color(26, 30, 44));

            drawText("TACTICS", 16, 10, 22, sf::Color(255, 230, 120));
            drawText("M/A/S/W: Move / Attack / Skill / Wait    R: restart", 16, 40, 13, sf::Color(150, 160, 180));
            if (!fontOk)
                window.setTitle("Tactics (no font found - put a .ttf named font.ttf next to the executable)");

            auto hl = computeHighlights();
            for (int y = 0; y < MAP_H; ++y)
                for (int x = 0; x < MAP_W; ++x) drawTile(x, y, hl);

            for (auto& f : floats) {
                float a = std::clamp(1.3f - f.t, 0.f, 1.f);
                sf::Color c = f.c; c.a = (sf::Uint8)(255 * std::min(1.f, a * 1.5f));
                drawText(f.s, f.pos.x, f.pos.y, 22, c, true);
            }

            drawText(hintText(), 16, 568, 15, sf::Color(230, 230, 170));
            for (size_t i = 0; i < logs.size(); ++i)
                drawText(logs[i], 16, 596.f + i * 18.f, 14,
                    i + 1 == logs.size() ? sf::Color(235, 235, 240) : sf::Color(150, 155, 170));

            drawHUD();

            if (mode == Mode::Over) {
                sf::RectangleShape ov({ (float)WIN_W, (float)WIN_H });
                ov.setFillColor(winner == 1 ? sf::Color(20, 80, 30, 150) : sf::Color(90, 20, 20, 150));
                window.draw(ov);
                drawText(winner == 1 ? "VICTORY" : "DEFEAT", 390, 290, 64,
                    winner == 1 ? sf::Color(140, 255, 160) : sf::Color(255, 130, 120), true);
                drawText("Press R to play again", 390, 370, 22, sf::Color::White, true);
            }
            window.display();
        }
    };

}  // namespace

int main() {
    Game game;
    game.run();
    return 0;
}