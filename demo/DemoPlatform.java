package demo;
import ecomm.Platform;
import ecomm.Seller;
import ecomm.Product;
import ecomm.Globals;
import java.util.*;
import java.io.*;
import java.io.BufferedWriter;
//import java.io.File;
//import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
//import java.io.OutputStream;
//import java.io.PrintWriter;;

public class DemoPlatform extends Platform{
    private ArrayList<Seller> sellers=new ArrayList<>();
    static int requestID=0;
    public boolean addSeller(Seller seller){
        //System.out.println("In Plaform add Seller");
        sellers.add(seller);
        return true;
    }
    //start //list category sortorder //Buy productid Numitems //Check 
    public static void clear(){
        try{
        BufferedWriter bw=new BufferedWriter(new FileWriter("PortalToPlatform.txt"));
        bw.write("");
    }
    catch(IOException e){
        
    }
    }
    public void processRequests(){
        try{
            //File file=new File("PortalToPlatform.txt");
            FileReader read=new FileReader("PortalToPlatform.txt");
            BufferedReader br=new BufferedReader(read);
            String s;
            BufferedWriter bw=new BufferedWriter(new FileWriter("PlatformToPortal.txt",true));
            //PrintWriter pw=new PrintWriter("PortalToPlatform.txt");
            ArrayList<Product> prod=new ArrayList<>();
            while((s=br.readLine())!=null){
                requestID++;
                //here splitting of s should be done
                String [] str=s.split(" ");//change in index is still not done
                //here s should be in the form of  listing/buy in the first index{asuming} but first index should be for requestID
                if(str[2].equals("Start")){
                    bw.write(str[0]+" "+str[1]+" "+"Mobile Book\n");
                    //System.out.println(str[0]+" "+str[1]+" "+"Mobile Book\n");
                }
                else if(str[2].equals("List")){
                    //System.out.println("Listing");
                    prod=this.findProduct(str[3]);
                    for(Product pr:prod){
                        //System.out.println("\n");
                        //System.out.println(pr.getName());
                        bw.write(str[0]+" "+str[1]+" "+pr.getName()+" "+pr.getProductID()+" "+pr.getPrice()+" "+pr.getQuantity()+"\n");//here writing about the product has been done 
                    }
                }
                else if(str[2].equals("Buy")){
                    String productID=str[3];
                    String sellerID=productID.split("-")[0];
                    Seller obj=null;
                    for(Seller seller: sellers){
                        if(seller.getID().equals(sellerID)){
                            obj=seller;
                            break;
                        }
                    }
                    if(obj==null||!obj.buyProduct(productID,Integer.parseInt(str[4]))){
                        bw.write(str[0]+" "+str[1]+" "+"Request Failed \n");
                    }
                    else{
                        bw.write(str[0]+" "+str[1]+" "+"Request Successfull \n");
                    }
                }
            }
            //file.delete();
            DemoPlatform.clear();
            br.close();
            bw.close();
        }
        catch(IOException e){
            System.out.println("Error in filereading");
        }

    }

    public ArrayList<Product> findProduct(String category){
        //here we will call method find products of sellers
        ArrayList<Product> products=new ArrayList<>();
        Globals.Category m=null;//*******Why to initialize with this Mobile???????******* Try to remove it
        if(category.equals("Mobile")){
            m=Globals.Category.Mobile;
            //System.out.println("Mobile1");
        }
        else if(category.equals("Book")){
            m=Globals.Category.Book;
        }
        for(int i=0;i<sellers.size();i++){
            ArrayList<Product> adprod=sellers.get(i).findProducts(m);
            for(int j=0;j<adprod.size();j++){
                products.add(adprod.get(j));
            }
        }
        //System.out.println(products);
        return products;
    }
}