#include <iostream>
#include "mixins/mixins.h"

template <typename> class Footballer;
template <typename> class Scientist;

template <typename M>
class Human : public mixin<M, requires<>, friends_with<Footballer, Scientist>> {
    // allow mixins in the composition to access private data.
    // type of the mixin is unkown to the mixin class, so it uses the mutator
    // to generate the type using the friend_t type function
    friend typename M::template friend_t<Footballer>;
    friend typename M::template friend_t<Scientist>;
public:
    Human() {
        std::cout << "Human constructor\n";
        health = 100;
        wealth = 100;
    }

    Human(const Human& obj) {
        std::cout << "Human copy construtctor\n";
        health = obj.health;
        wealth = obj.wealth;
    }

    Human(Human&& obj) {
        std::cout << "Human move construtctor\n";
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
public:
    Footballer(const Footballer& obj) : Footballer() {
        std::cout << "Footballer copy constructor\n";
        score = obj.score;
    }

    Footballer(Footballer&& obj) : Footballer() {
        std::cout << "Footballer move constructor\n";
        score = obj.score;
    }

    // initialize mixins references at construction
    // constructor delegation can ensure that it's initialized
    Footballer() : human_mixin(this->template mutate<Human>()) {
        std::cout << "Footballer constructor\n";
    }

    void score_goal() {
        std::cout << "Goaaal!";
        score++;
    }

    void pass() {
        std::cout << "I gained money for passing!\n";
        // since Footballer is a friend of Human, it can access its private data
        human_mixin.money += 100;
    }

    void shoot() {
        std::cout << "I gained money for shooting!!\n";
        human_mixin.money += 200;
    }

    virtual ~Footballer() {
        std::cout << "Footballer destructed!\n";
    }

private:
    int score;
    // requirement mixins can be member variables
    typename M::template requirement_t<Human>& human_mixin;
};

template <typename M>
class Scientist : public mixin<M, requires<Human>> {
public:
    Scientist(const Scientist& obj) : Scientist() {
        std::cout << "Scientist copy constructor\n";
        citations = obj.citations;
    }

    Scientist(Scientist&& obj) : Scientist() {
        std::cout << "Scientist move constructor\n";
        citations = obj.citations;        
    }

    Scientist() : human_mixin(this->template mutate<Human>()) {
        std::cout << "Scientist Constructor\n";
    }

    void publish() {
        citations++;
        human_mixin.wealth += 1;
    }

    virtual ~Scientist() {
        std::cout << "Scientist Destructed!\n";
    };

private:
    typename M::template requirement_t<Human>& human_mixin;
    int citations;
};

int main() {
    // doesn't compile since they require the Human mixin 
    // composition<Scientist, Footballer> scientist_footballer;
    composition<Human> man;
    composition<Human, Scientist, Footballer> mega_man;
    
    std::cout << mega_man.get_wealth() << '\n';
    // Scientist mixin changed the Human mixin wealth member variable 
    mega_man.publish();
    std::cout << mega_man.get_wealth() << '\n';

    auto mega_man2 = mega_man;
    std::cout << mega_man2.get_wealth() << '\n';
}