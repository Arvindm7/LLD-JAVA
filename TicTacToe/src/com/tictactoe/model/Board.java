package TicTacToe.src.com.tictactoe.model;

public class Board {
    
    private final char[][] cells = new char[3][3];

    public Board()
    {
        for(int i = 0 ; i < 3 ; i++)
        {
            for(int j = 0 ; j < 3 ; j++)
            {
                cells[i][j] = ' ';
            }
        }
    }

    public void placeMark(int row,int col, char symbol)
    {
        if(row < 0 || row >= 3 || col < 0 || col >= 3)
        {
            throw new IllegalArgumentException(
                "Position is out of the board"
            );
        }

        if(symbol != 'X' && symbol != 'O')
        {
            throw new IllegalArgumentException(
                "Invalid symbol"
            );
        }

        if(cells[row][col] != ' ')
        {
            throw new IllegalArgumentException(
                "Cell is already occupied"
            );
        }
        cells[row][col] = symbol;
    }

    public char[][] getCells()
    {
        return cells;
    }

    public boolean hasWon(char symbol)
    {
        for(int i = 0 ; i < 3 ; i++)
        {
            if(cells[0][i] == symbol && cells[1][i] == symbol && cells[2][i] == symbol)
            {
                return true;
            }
            if(cells[i][0] == symbol && cells[i][1] == symbol && cells[i][2] == symbol)
            {
                return true;
            }
        }

        if(cells[0][0] == symbol && cells[1][1] == symbol && cells[2][2] == symbol)
        {
            return true;
        }

        if(cells[2][0] == symbol && cells[1][1] == symbol && cells [0][2] == symbol)
        {
            return true;
        }
        return false;
    }

    public boolean isFull()
    {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (cells[row][col] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }
    public void printBoard()
    {
        System.out.println("-------------");
        for(int row = 0 ; row < 3 ; row++)
        {
            System.out.print("| ");
            for(int col = 0 ; col < 3; col++)
            {
                System.out.print(cells[row][col] + " | ");
            }
            System.out.println();
            System.out.println("-------------");
        }
    }
}
