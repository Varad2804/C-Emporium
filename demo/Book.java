package demo;
import ecomm.Product;
import ecomm.Globals;
public class Book extends Product{
    private String productID;
    private float price;
    private int quantity;
    private String sellerID;
    private static int countBook=0;
    private String Name;
    public Book(String Name,float price, int quantity ,String productID){
        //System.out.println("In Book "+countBook);
        countBook++;
        this.Name=Name;
        this.price=price;
        this.quantity=quantity;
        this.productID=productID;
        //System.out.println("\n" +productID);
    }
    public void setSellerID(String ID){
        this.sellerID=ID;
        this.productID=ID+"-"+this.productID;
    }
    public void Buy(int quantity){
        this.quantity-=quantity;
    }
    
    public Globals.Category getCategory(){
        return Globals.Category.Book;
    }

    public String getName(){
        return Name;
    }

    public String getSellerID(){
        return sellerID;
    }
    
    public String getProductID(){
        return productID;
    }

    public float getPrice(){
        return price;
    }

    public int getQuantity(){
        return quantity;
    }
}