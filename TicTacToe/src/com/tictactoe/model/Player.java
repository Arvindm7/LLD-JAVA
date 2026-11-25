package TicTacToe.src.com.tictactoe.model;

public class Player {

    private final String name;
    private final char symbol;

    public Player(String name, char symbol)
    {
        if(name == null || name.isEmpty())
        {
            throw new IllegalArgumentException(
                "Player Name can not be empty"
            );
        }
        
        if(symbol != 'X' && symbol != 'O')
        {
            throw new IllegalArgumentException(
                "Symbol can only be X or O"
            );
        }

        this.name = name;
        this.symbol = symbol;
    }

    public String getName()
    {
        return name;
    }

    public char getSymbol()
    {
        return symbol;
    }
    
}
