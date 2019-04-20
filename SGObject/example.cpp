#include <iostream>
#include "mixins/mixins.h"

template <typename> class Footballer;
template <typename> class Scientist;

template <typename M>
class Human : public mixin<M, requires<>, friends_with<Footballer, Scientist>> {
    friend typename M::template friend_t<Footballer>;
    friend typename M::template friend_t<Scientist>;
public:
    Human() {
        std::cout << "Human Constructor\n";
        health = 100;
        wealth = 100;
    }

    Human(const Human& obj) {
        std::cout << "Human Copy Constructed\n";
        health = obj.health;
        wealth = obj.wealth;
    }

    void breathe() {
        std::cout << "I'm breathing\n";
        health++;
    }

    void eat() {
        std::cout << "I'm eating\n";
        health += 5;
    }

    void drink() {
        std::cout << "I'm drinking\n";
        health += 3;
    }

    int get_health() { return health; }
    int get_wealth() { return wealth; }

    virtual ~Human() {
        std::cout << "Human destructed!\n";
    }
private:
    int health;
    int wealth;
};

template <typename M>
class Footballer : public mixin<M, requires<Human>> {
private:
    typename M::template requirement_t<Human>* human_obj;
public:
    Footballer() : human_obj(this->template mutate<Human>()) {
        std::cout << "Footballer Constructor\n";
    }

    Footballer(const Footballer& obj) {
        std::cout << "Footballer Copy Constructed\n";
        score = obj.score;
    }

    void score_goal() {
        std::cout << "Goaaal!";
        score++;
    }

    void pass() {
        std::cout << "I gained money for passing!\n";
        human_obj->wealth += 500;
    }

    void shoot() {
        std::cout << "I gained money for shooting!!\n";
        human_obj->wealth += 1000;
    }

    virtual ~Footballer() {
        std::cout << "Footballer destructed!\n";
    }

private:
    int score;
};

template <typename M>
class Scientist : public mixin<M, requires<Human>> {
    typename M::template requirement_t<Human>* human_obj;
public:
    Scientist() : human_obj(this->template mutate<Human>()) {
        std::cout << "Scientist Constructor\n";
    }

    Scientist(const Scientist& obj) {
        std::cout << "Scientist Copy Constructed\n";
        citations = obj.citations;
    }

    void publish() {
        citations++;
        human_obj->wealth += 1;
    }

    virtual ~Scientist() {
        std::cout << "Scientist Destructed!\n";
    };

private:
    int citations;
};

int main() {
    // doesn't compile
    // composition<Scientist, Footballer> mega_man;
    // doesn't compile
    // composition<Footballer> mega_man;
    composition<Human> man;
    composition<Human, Scientist, Footballer> mega_man;

    mega_man.publish();
    std::cout << mega_man.get_wealth() << '\n';

    man.eat();
    std::cout << man.get_health() << '\n';
}