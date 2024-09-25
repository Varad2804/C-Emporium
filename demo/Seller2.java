package demo;
import java.util.ArrayList;
import ecomm.Product;
import ecomm.Seller;
import ecomm.Platform;
import ecomm.Globals;
public class Seller2 extends Seller{
    private ArrayList<Product> product=new ArrayList<>();
    private String myID;
    public Seller2(String ID){
        super(ID);
        myID=ID;
    }
    public void addproduct(Product obj){
        product.add(obj);
        if(obj.getCategory()==Globals.Category.Mobile){
            Mobile obj1=(Mobile)obj;
            obj1.setSellerID(myID);
        }
        else if(obj.getCategory()==Globals.Category.Book){
            Book obj1=(Book)obj;
            obj1.setSellerID(myID);
        }
    }
    public void addPlatform(Platform object){
        object.addSeller(this);
    }
    
    public ArrayList<Product> findProducts(Globals.Category whichone){
        ArrayList<Product> products=new ArrayList<>();
        for(Product m:product){
            if(m.getCategory()==whichone){
                products.add(m);
            }
        }
        return products;
    }
    
    public boolean buyProduct(String productID,int quantity){
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