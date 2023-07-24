# Frogger
Play a text-based game of frogger inside the terminal

You win by reaching a lilypad, but if you die 3 times and lose all your lives then you lose  
Move frogger with wasd

## Initialisation

Start by creating the game board with 5 spread out lilypads on one side, and the river bank on the other, with the Frogger positioned in the middle  
Place turtles in the initialisation phase by typing in the amount of turtles, followed by the coordinates of each turtle  
E.g. 3 4 5 2 3 1 2 makes 3 turtles, positioned at (4,5), (2,3), and (1,2)

## Commands

After every command the current state of the board will be printed out

- Add a log after initialisation using the l command  
`l [row] [start_column] [end_column]`

- Clear a row of all obstacles and enemies (excluding the bank row, lilypad row, and the current row of frogger)  
`c [row]`

- Delete a log by targetting a coordinate with a log tile  
`r [row] [column]`

- Add enemy bugs that move sideways everytime the frogger moves, but they can only stand on logs or turtles  
`b [row] [column]`

- A game mode to toggle the movement of the game rows as you move, but also turns off the functionality to edit the board  
`g [1/0]`

- A superbug that finds the shortest path to the player after every move and moves along it  
`z [row] [col]`
