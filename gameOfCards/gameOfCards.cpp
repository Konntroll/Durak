// gameOfCards.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <array>
#include <vector>
#include <ctime>
#include <cstdlib>

class Card {
public:
	enum Ranks {
		RANK_6, //0
		RANK_7, //1
		RANK_8, //2
		RANK_9, //3
		RANK_10, //4
		RANK_JACK, //5
		RANK_QUEEN, //6
		RANK_KING, //7
		RANK_ACE, //8
		MAX_RANKS //9
	};

	enum Suits {
		SUIT_HEARTS,
		SUIT_CLUBS,
		SUIT_DIAMONDS,
		SUIT_SPADES,
		MAX_SUITS
	};

private:
	Ranks m_rank;
	Suits m_suit;

public:
	Card(Ranks rank = RANK_6, Suits suit = SUIT_HEARTS) : m_rank(rank), m_suit(suit) {
		//default constructor
	}

	Ranks getRank() {
		return m_rank;
	}

	Suits getSuit() {
		return m_suit;
	}

	const int getCardValue() {
		return m_rank + 6;
	}

	const void printCard() {
		switch (m_rank) {
		case RANK_6:
			std::cout << 6;
			break;
		case RANK_7:
			std::cout << 7;
			break;
		case RANK_8:
			std::cout << 8;
			break;
		case RANK_9:
			std::cout << 9;
			break;
		case RANK_10:
			std::cout << 10;
			break;
		case RANK_JACK:
			std::cout << "J";
			break;
		case RANK_QUEEN:
			std::cout << "Q";
			break;
		case RANK_KING:
			std::cout << "K";
			break;
		case RANK_ACE:
			std::cout << "A";
			break;
		}
		switch (m_suit) {
		case SUIT_HEARTS:
			std::cout << "H";
			break;
		case SUIT_CLUBS:
			std::cout << "C";
			break;
		case SUIT_DIAMONDS:
			std::cout << "D";
			break;
		case SUIT_SPADES:
			std::cout << "S";
			break;
		}
	}
};

class Deck {
	std::array<Card, 36> m_deck;
	int m_cardIndex = 0;

public:
	Deck() {
		int rank = 0;
		int suit = 0;
		for (int index = 0; index < 36; index++) {
			m_deck[index] = Card(static_cast<Card::Ranks>(rank), static_cast<Card::Suits>(suit));
			rank++;
			if (rank == Card::MAX_RANKS) {
				rank = 0;
				suit++;
			}
		}
	}

	void shuffleDeck() {
		for (int index = 0; index < 36; index++) {
			m_cardIndex = 0;
			static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
			int randCard = static_cast<int>(rand() * fraction * (36));
			cardSwap(m_deck[index], m_deck[randCard]);
		}
	}

	friend static void cardSwap(Card &cardA, Card &cardB) {
		Card temp = cardA;
		cardA = cardB;
		cardB = temp;
	}

	Card dealCard() {
		return m_deck[m_cardIndex++];
	}

	int cardsLeft() {
		return m_cardIndex;
	}
};

class Game {
	std::vector<Card> playerHand;
	std::vector<Card> oppHand;
	std::vector<Card> table;
	Deck gameDeck;
	Card trump;
	bool outOfCards;

public:
	Game() {
		gameDeck.shuffleDeck();
		while (oppHand.size() != 6) {
			playerHand.push_back(gameDeck.dealCard());
			oppHand.push_back(gameDeck.dealCard());
		}

		trump = gameDeck.dealCard();
		std::cout << "The trump card is: ";
		trump.printCard();
		std::cout << std::endl;

		outOfCards = false;

		playerPlay();
		return;
	}

	void playerPlay() {
		checkVictory(playerHand, false);
		std::cout << "It's your turn.";
		printHand();
		int cardIndex;
		if (table.size() == 0) {
			std::cout << " Enter a number corresponding to the card you want to play." << std::endl;
			std::cin >> cardIndex;
			inputCheck(cardIndex);
			table.push_back(playerHand[cardIndex - 1]);
			playerHand.erase(playerHand.begin() + (cardIndex - 1));
			oppReact(); //this calls for opponent to respond to the player's initial action for the turn
			return;
		}
		else {
			printTable();
			bool match = false;
			while (match == false) {
				std::cout << "Enter a number corresponding to the card you want to add or 0 if you want to pass." << std::endl;
				std::cin >> cardIndex;
				inputCheck(cardIndex);
				if (cardIndex == 0) {
					match = true;
					endTurn(false);
					checkVictory(playerHand, false);
					checkVictory(oppHand, true);
					oppPlay(); //pass the turn to the opponent
					//return;
				}
				else {
					for each(Card card in table) {
						if (playerHand[cardIndex - 1].getRank() == card.getRank()) {
							match = true;
							break; //to skip checking other cards on the table if a match was already found
						}
					}
					if (match == true) {
						table.push_back(playerHand[cardIndex - 1]);
						playerHand.erase(playerHand.begin() + (cardIndex - 1));
						oppReact();
						return;
					}
					else {
						std::cout << "Sorry, this doesn't match anything on the table." << std::endl;
						continue;
					}
				}
			}
		}
	}

	void playerReact() {
		checkVictory(playerHand, false);
		printTable();
		bool match = false;
		while (match == false) {
			printHand();
			std::cout << "\nYou need to answer ";
			table.back().printCard();
			std::cout << std::endl;
			std::cout << "\nEnter a number that corresonds to the card you want to answer with or 0 if you have nothing." << std::endl;
			int cardIndex;
			std::cin >> cardIndex;
			inputCheck(cardIndex);
			if (cardIndex == 0) {
				match = true;
				addCards(false);
				while (table.size() != 0) { //clear the table if the turn resolved without the opponent failing to answer
					playerHand.push_back(table.back());
					table.pop_back();
				}
				drawCards(oppHand); //refill the opponent's hand
				checkVictory(oppHand, true);
				oppPlay(); //pass the turn to the opponent
				//return;
			}
			else {
				if ((playerHand[cardIndex - 1].getRank() > table.back().getRank() && playerHand[cardIndex - 1].getSuit() == table.back().getSuit()) ||
					(playerHand[cardIndex - 1].getSuit() == trump.getSuit() && table.back().getSuit() != trump.getSuit())) {
					table.push_back(playerHand[cardIndex - 1]);
					playerHand.erase(playerHand.begin() + (cardIndex - 1));
					match = true;
				}
				else {
					std::cout << "Sorry, this doesn't match the card you have to answer." << std::endl;
					continue;
				}
			}
			if (playerHand.size() > 0) {
				oppPlay();
				return;
			}
			else {
				endTurn(true);
				playerPlay();
				return;
			}
			//return;
		}
	}

	void oppPlay() {
		checkVictory(oppHand, true);
		std::vector<Card> playables;
		if (table.size() == 0) {
			for each (Card card in oppHand) {
				if (card.getSuit() != trump.getSuit()) {
					playables.push_back(card);
				}
			}
			if (playables.size() != 0) {
				sortPlaybles(playables);
				printPlay(playables[0]);
				table.push_back(playables[0]);
				removeCard(oppHand, playables[0]);
				playerReact();
				return;
			}
			else {
				sortPlaybles(oppHand);
				printPlay(oppHand[0]);
				table.push_back(oppHand[0]);
				oppHand.erase(oppHand.begin() + 0);
				playerReact();
				return;
			}
		}
		else {
			for (unsigned int card = 0; card < oppHand.size(); card++) {
				for each (Card playedCard in table) {
					if (oppHand[card].getRank() == playedCard.getRank() && (oppHand[card].getSuit() != trump.getSuit())) {
						playables.push_back(oppHand[card]);
					}
				}
			}
			if (playables.size() != 0) {
				sortPlaybles(playables);
				printPlay(playables[0]);
				table.push_back(playables[0]);
				removeCard(oppHand, playables[0]);
				playerReact();
				return;
			}
			else {
				std::cout << "That's it.\n" << std::endl;
				endTurn(true);
				checkVictory(oppHand, true);
				checkVictory(playerHand, false);
				playerPlay();
				//return;
			}
		}
	}

	void oppReact() {
		checkVictory(oppHand, true);
		std::vector<Card> playables;
		//populate the array above with suitable non-trump options
		for each (Card card in oppHand) {
			if (card.getRank() > table.back().getRank() && card.getSuit() == table.back().getSuit()) { //compares the last unbeaten card on the table to those in the opp's hand
				playables.push_back(card);
			}
		}
		if (playables.size() > 1) {
			sortPlaybles(playables); //sort playables by strength in ascending order
			printAnswer(table.back(), playables[0]);
			table.push_back(playables[0]); //play the least strong playable card
			removeCard(oppHand, playables[0]);
			if (oppHand.size() > 0) {
				playerPlay();
				return;
			}
			else {
				endTurn(false);
				oppPlay();
				return;
			}
		}
		else if (playables.size() == 1) {
			printAnswer(table.back(), playables[0]);
			table.push_back(playables[0]); //play the only playable card
			removeCard(oppHand, playables[0]);
			if (oppHand.size() > 0) {
				playerPlay();
				return;
			}
			else {
				endTurn(false);
				oppPlay();
				return;
			}
		}
		else { //if none are available, try trumps
			for each (Card card in oppHand) {
				if (card.getRank() > table.back().getRank() && card.getSuit() == trump.getSuit()) {
					playables.push_back(card);
				}
			}
		}
		if (playables.size() > 1) {
			sortPlaybles(playables); //sort playable trumps by strength in ascending order
			printAnswer(table.back(), playables[0]);
			table.push_back(playables[0]); //play the least strong playable trump
			removeCard(oppHand, playables[0]);
			if (oppHand.size() > 0) {
				playerPlay();
				return;
			}
			else {
				endTurn(false);
				oppPlay();
				return;
			}
		}
		else if (playables.size() == 1) {
			printAnswer(table.back(), playables[0]);
			table.push_back(playables[0]); // play the only available trump
			removeCard(oppHand, playables[0]);
			if (oppHand.size() > 0) {
				playerPlay();
				return;
			}
			else {
				endTurn(false);
				oppPlay();
				return;
			}
		}
		else { //if no playable trumps are available, take all the cards on the table and empty it
			std::cout << "I have nothing, have mercy." << std::endl;
			addCards(true); //the player adds cards, max opp hand size - 1
			while (table.size() != 0) { //clear the table if the turn resolved without the opponent failing to answer
				oppHand.push_back(table.back());
				table.pop_back();
			}
			drawCards(playerHand); //refill the player's hand
			if (playerHand.size() == 0) {
				std::cout << "You win! Wanna play again? (y/n) ";
				char play;
				std::cin >> play;
				if (play == 'y') {
					Game newGame;
				}
				else throw 1;
			}
			playerPlay(); //the player goes again
			return;
		}
	}

	void addCards(bool oppFail) {
		if (oppFail == true) {
			printHand();
			printTable();
			int limit = oppHand.size() - 1;
			while (limit > 0) {
				int cardIndex;
				std::cout << "Enter a number corresponding to the card you want to add or 0 if you want to pass." << std::endl;
				std::cin >> cardIndex;
				inputCheck(cardIndex);
				if (cardIndex == 0) return;
				else {
					for each(Card card in table) {
						if (playerHand[cardIndex - 1].getRank() == card.getRank()) {
							table.push_back(playerHand[cardIndex - 1]);
							playerHand.erase(playerHand.begin() + (cardIndex - 1));
							limit--;
							break; //to skip checking other cards on the table if a match was already found
						}
						else {
							std::cout << "Sorry, this card doesn't match anything on the table." << std::endl;
						}
					}
				}
			}
		}
		else {
			std::vector<Card> playables;
			for (unsigned int card = 0; card < oppHand.size(); card++) {
				for each (Card playedCard in table) {
					if (oppHand[card].getRank() == playedCard.getRank() && oppHand[card].getSuit() != trump.getSuit()) {
						playables.push_back(oppHand[card]);
					}
				}
			}
			if (playables.size() > 0) {
				int limit = playerHand.size() - 1;
				while (limit > 0) {
					table.push_back(playables[0]);
					removeCard(oppHand, playables[0]);
					playables.erase(playables.begin() + 0);
					if (playables.size() == 0) { //return to caller if the number of playables is less than the limit
						return;
					}
					limit--;
				}
			}
		}
	}

	void drawCards(std::vector<Card> &hand) {
		while (hand.size() < 6 && outOfCards == false) {
			if (gameDeck.cardsLeft() != 36) {
				hand.push_back(gameDeck.dealCard());
			}
			else if (gameDeck.cardsLeft() == 36 && hand.size() < 6) {
				hand.push_back(trump);
				outOfCards = true;
			}
		}
	}

	void sortPlaybles(std::vector<Card> &playables) {
		for (unsigned int pass = 0; pass < playables.size(); pass++) {
			for (unsigned int card = 0; card < playables.size() - 1; card++) {
				if (playables[card].getRank() > playables[card + 1].getRank()) {
					cardSwap(playables[card], playables[card + 1]);
				}
			}
		}
	}

	void removeCard(std::vector<Card> &hand, Card playedCard) {
		for (unsigned int card = 0; card < hand.size(); card++) {
			if (hand[card].getRank() == playedCard.getRank() && hand[card].getSuit() == playedCard.getSuit()) {
				hand.erase(hand.begin() + card);
				return;
			}
		}
	}

	void printAnswer(Card toBeat, Card beatWith) {
		std::cout << "\nAnswer ";
		toBeat.printCard();
		std::cout << " with ";
		beatWith.printCard();
		std::cout << ".\n";
	}

	void printPlay(Card playCard) {
		std::cout << "\nI'll play ";
		playCard.printCard();
		std::cout << ".\n";
	}

	void printHand() {
		std::cout << "\nYou have: ";
		int cardIndex = 0; //this is to give the cards in the player's hand numbers for choice inputs
		for each (Card card in playerHand) {
			std::cout << " (" << ++cardIndex << ") ";
			card.printCard();
		}
	}

	void printTable() {
		if (table.size() == 0) return;
		std::cout << std::endl;
		if (table.size() == 1) {
			table.back().printCard();
			std::cout << " is the only card on the table. " << std::endl;
			return;
		}

		for (unsigned int card = 0; card < table.size(); card++) {
			if (card == table.size() - 1 && table.size() % 2 == 1) {
				std::cout << "And ";
				table[card].printCard();
				std::cout << " is the last card on the table." << std::endl;
			}
			else if (card % 2 == 0) {
				table[card].printCard();
				std::cout << " is covered by ";
			}
			else {
				table[card].printCard();
				std::cout << std::endl;
			}
		}
	}

	void endTurn(bool oppTurn) {
		while (table.size() != 0) {
			table.pop_back();
		}
		if (oppTurn == false) {
			drawCards(playerHand);
			drawCards(oppHand);
		}
		else {
			drawCards(oppHand);
			drawCards(playerHand);
		}
	}
	
	void checkVictory(std::vector<Card> hand, bool oppWin) {
		if (hand.size() == 0 && oppWin == false) {
			std::cout << "You win! Wanna play again? (y/n) ";
			char play;
			std::cin >> play;
			if (play == 'y') {
				Game newGame;
			}
			else throw 1;
		}
		else if (hand.size() == 0 && oppWin == true) {
			std::cout << "Sorry, you lose! Wanna play again? (y/n) ";
			char play;
			std::cin >> play;
			if (play == 'y') {
				Game newGame;
			}
			else throw 1;
		}
	}

	void inputCheck(int &arrIndex) {
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(32767, '\n');
			std::cout << "Wrong index. Please try again." << std::endl;
			std::cin >> arrIndex;
			inputCheck(arrIndex);
		}
		if (arrIndex > static_cast<int>(playerHand.size()) || arrIndex < 0) {
			std::cout << "Wrong index. Please try again." << std::endl;
			std::cin >> arrIndex;
			inputCheck(arrIndex);
		}
	}
};

int main()
{
	srand(static_cast<unsigned int>(time(0))); // set initial seed value to system clock
	rand(); // If using Visual Studio, discard first random value

	try {
		Game newGame;
	}
	catch (int) {
		return 0;
	}

	return 0;
}