#ifndef WORD_H
#define WORD_H

#include "tools.h"


typedef std::string generatorName;
typedef int generatorIndex;
typedef std::pair<generatorIndex, int> letter;

class Word
{
    friend Word operator*(Word w1, const Word &w2);
    friend std::ostream & operator <<(std::ostream &out, const Word &w);

public:
    Word(const std::vector<letter> letters = {});

    letter & operator[](std::vector<letter>::size_type i);
    letter operator[](std::vector<letter>::size_type i) const;
    void clear();
    void push_back(const letter &l);
    letter & back();
    void reserve(std::vector<letter>::size_type N);
    std::vector<letter>::size_type size() const;

    const std::vector<letter> & getLetters() const;

    static Word contract(const Word &w);
    static std::vector<Word> contract(std::vector<Word> V);
    void contract();
    Word inverse() const;

private:
    std::vector<letter> letters;
};

#endif // WORD_H
