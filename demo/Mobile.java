package demo;
import ecomm.Product;
import ecomm.Globals;
public class Mobile extends Product{
    private String productID;
    private float price;
    private int quantity;
    private String sellerID;
    private static int countMobile=0;
    private String Name;
    public Mobile(String Name,float price, int quantity ,String productID){
        //System.out.println("In Mobile "+countMobile);
        this.Name=Name;
        countMobile++;
        this.price=price;
        this.quantity=quantity;
        this.productID=productID;
        //System.out.println("\n" +productID);
    }
    public void Buy(int quantity){
        this.quantity-=quantity;
    }
    void setSellerID(String ID){
        this.sellerID=ID;
        //System.out.println(sellerID+" "+Name);
        this.productID=ID+"-"+this.productID;
    }
    public Globals.Category getCategory(){
        return Globals.Category.Mobile;
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