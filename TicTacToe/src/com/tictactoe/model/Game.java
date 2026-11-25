package TicTacToe.src.com.tictactoe.model;

public class Game {

    private final Board board;
    private final Player[] player;
    private int currentPlayerIndex;
    private GameStatus gameStatus;

    public Game(Player player1, Player player2)
    {
        if(player1 == null || player2 == null)
        {
            throw new IllegalArgumentException(
                "Players can not be null."
            );
        }

        if(player1.getSymbol() == player2.getSymbol())
        {
            throw new IllegalArgumentException(
                "Both Players can not have same symbol"
            );
        }

        this.board = new Board();
        this.player = new Player[]{player1,player2};
        this.currentPlayerIndex = 0;
        this.gameStatus = GameStatus.IN_PROGRESS;

    }

    public Board getBoard()
    {
        return board;
    }

    public Player getCurrentPlayer()
    {
        return player[currentPlayerIndex];
    }

    public GameStatus getGameStatus()
    {
        return gameStatus;
    }

    public void makeMove(int row, int col)
    {
        if(gameStatus != GameStatus.IN_PROGRESS)
        {
            throw new IllegalStateException(
                "Game has already ended or drawed"
            );
        }

        Player currPlayer = getCurrentPlayer();

        //make mark
        board.placeMark(row, col, currPlayer.getSymbol());

        if(board.hasWon(currPlayer.getSymbol()))
        {
            if(currPlayer.getSymbol() == 'X')
            {
                gameStatus = GameStatus.X_WON;
                return;
            }
            else{
                gameStatus = GameStatus.O_WON;
                return;
            }
        }

        if(board.isFull())
        {
            gameStatus = GameStatus.DRAW;
            return;
        }
        currentPlayerIndex = (currentPlayerIndex + 1) % 2;
    }
    
}
