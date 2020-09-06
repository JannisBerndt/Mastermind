# Mastermind
Mastermind has originally been a guessing game for two players, but the second player can perfectly simulated by minor game logic which makes it effectively a singleplayer game.
I used Qt, a very popular Framework for building cross-platform GUI Applications with C++.

## How Mastermind is played
The Goal is to guess a color code of four colors in the correct order.  
To do that, you have 12 tries. After every try you are given some hints.  
For a correct color in the correnct place, you are given a black dot. For every color being in the code, but not at the right position, you get a white dot.  
With these information you have to find the correct code with less or equal to 12 tries, to win.
(Colors for the hints are varying from one game design to another)  
It is also possible to create custom playing modes to make it more difficult, or easier.
  
Feel free to download/fork or whatever and make you own adjustments and modes. :smiley:
