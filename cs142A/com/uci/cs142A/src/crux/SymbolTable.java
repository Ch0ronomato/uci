package crux;

import java.util.LinkedHashMap;
import java.util.Map;
public class SymbolTable {
    public static String studentName = "Ian Schweer";
    public static String studentID = "22514022";
    public static String uciNetID = "ischweer";
    private Map<String, Symbol> symbols;
    private SymbolTable parent;
    private int depth;
    public SymbolTable()
    {
        this.symbols = new LinkedHashMap<>();
        this.depth = 0;
        this.parent = null;
    }

    public SymbolTable(SymbolTable parent) {
        this.parent = parent;
        this.symbols = new LinkedHashMap<>();
        this.depth = parent.getDepth() + 1;
    }

    public int getDepth() {
        return depth;
    }

    public SymbolTable getParent() {
        return parent;
    }

    public Symbol lookup(String name) throws SymbolNotFoundError
    {
        if (symbols.containsKey(name)) {
            return symbols.get(name);
        } else {
            SymbolTable x = getParent();
            while (x != null) {
                if (x.symbols.containsKey(name))
                    return x.symbols.get(name);
                x = x.getParent();
            }
            throw new SymbolNotFoundError(name);
        }
    }
       
    public Symbol insert(String name) throws RedeclarationError
    {
        if (!symbols.containsKey(name)) {
            Symbol symbol = new Symbol(name);
            symbols.put(name, symbol);
            return symbol;
        } else {
            throw new RedeclarationError(symbols.get(name));
        }
    }
    
    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        if (parent != null)
            sb.append(parent.toString());
        
        String indent = new String();
        for (int i = 0; i < depth; i++) {
            indent += "  ";
        }
        
        for (Symbol s : this.symbols.values())
        {
            sb.append(indent + s.toString() + "\n");
        }
        return sb.toString();
    }
}

class SymbolNotFoundError extends Error
{
    private static final long serialVersionUID = 1L;
    private String name;
    
    SymbolNotFoundError(String name)
    {
        this.name = name;
    }
    
    public String name()
    {
        return name;
    }
}

class RedeclarationError extends Error
{
    private static final long serialVersionUID = 1L;

    public RedeclarationError(Symbol sym)
    {
        super("Symbol " + sym + " being redeclared.");
    }
}
