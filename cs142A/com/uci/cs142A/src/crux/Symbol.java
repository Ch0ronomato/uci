package crux;

public class Symbol {
    public static String studentName = "Ian Schweer";
    public static String studentID = "22514022";
    public static String uciNetID = "ischweer";
    private String name;

    public Symbol(String name) {
        this.name = name;
    }
    
    public String name()
    {
        return this.name;
    }
    
    public String toString()
    {
        return "Symbol(" + name + ")";
    }

    public static Symbol newError(String message) {
        return new ErrorSymbol(message);
    }
}

class ErrorSymbol extends Symbol
{
    public ErrorSymbol(String message)
    {
        super(message);
    }
}
