package TicTacToe.src.com.tictactoe.main;

import java.util.Scanner;

import TicTacToe.src.com.tictactoe.model.Game;
import TicTacToe.src.com.tictactoe.model.GameStatus;
import TicTacToe.src.com.tictactoe.model.Player;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.print("Enter Player 1 name (X): ");
        String name1 = scanner.nextLine();

        System.out.print("Enter Player 2 name (O): ");
        String name2 = scanner.nextLine();

        Player p1 = new Player(name1, 'X');
        Player p2 = new Player(name2, 'O');

        Game game = new Game(p1, p2);

        while (game.getGameStatus() == GameStatus.IN_PROGRESS) {
            game.getBoard().printBoard();

            Player current = game.getCurrentPlayer();

            System.out.println(
                current.getName() + "'s turn (" +
                current.getSymbol() + ")"
            );

            try {
                System.out.print("Enter row and column (0-2): ");

                int row = scanner.nextInt();
                int col = scanner.nextInt();

                game.makeMove(row, col);
            } catch (IllegalArgumentException |
                     IllegalStateException e) {
                System.out.println("Invalid move: " + e.getMessage());
            } catch (java.util.InputMismatchException e) {
                System.out.println(
                    "Please enter numeric row and column values."
                );
                scanner.nextLine();
            }
        }

        game.getBoard().printBoard();

        if (game.getGameStatus() == GameStatus.DRAW) {
            System.out.println("It's a draw!");
        } else {
            Player winner = game.getGameStatus() == GameStatus.X_WON
                    ? p1 : p2;

            System.out.println("Winner: " + winner.getName());
        }

        scanner.close();
    }

}
