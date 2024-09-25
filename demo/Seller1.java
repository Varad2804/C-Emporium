package demo;
import java.util.ArrayList;
import ecomm.Product;
import ecomm.Seller;
import ecomm.Platform;
import ecomm.Globals;
public class Seller1 extends Seller{
    private ArrayList<Product> product=new ArrayList<>();
    private String myID;
    //private static int countSeller=0;
    public Seller1(String ID){
        super(ID);
        //System.out.println("In Seller1");
        myID=ID;
        //countSeller++;
    }
    public void addproduct(Product obj){
        //System.out.println(product.size());
        product.add(obj);
        if(obj.getCategory()==Globals.Category.Mobile){
            Mobile obj1=(Mobile)obj;
            //System.out.println(myID+" MyID");
            obj1.setSellerID(myID);
        }
        else if(obj.getCategory()==Globals.Category.Book){
            Book obj1=(Book)obj;
            obj1.setSellerID(myID);
        }
    }
    public void addPlatform(Platform object){
       // System.out.println("In add platform of seller 1");
        object.addSeller(this);
    }
    
    public ArrayList<Product> findProducts(Globals.Category whichone){
        //System.out.println("In find products of seller1");
        ArrayList<Product> products=new ArrayList<>();
        for(Product m:product){
            if(m.getCategory()==whichone){
                products.add(m);
            }
        }
        //System.out.println(products+"In Seller1");
        return products;
    }
    
    public boolean buyProduct(String productID,int quantity){
        //System.out.println("Buy Product");
        boolean flag=false;
        for(Product prod:product){
            if(prod.getProductID().equals(productID)){
                flag=true;
                if(prod.getCategory().equals(Globals.Category.Mobile)){
                    Mobile obj=(Mobile)prod;
                    if(obj.getQuantity()>0 && obj.getQuantity()>=quantity){
                    obj.Buy(quantity);
                    if(obj.getQuantity()==0){
                        product.remove(obj);
                    }
                }
                    else flag=false;
                    break;
                }
                else if(prod.getCategory().equals(Globals.Category.Book)){
                    Book obj=(Book)prod;
                     if(obj.getQuantity()>0 && obj.getQuantity()>=quantity){
                    obj.Buy(quantity);
                    if(obj.getQuantity()==0){
                        product.remove(obj);
                    }
                }
                    else flag=false;
                    break;
                }
            }
        }
        return flag;
    }
}