#include<bits/stdc++.h>

using namespace std;

class Product{
    private:
    int sku;
    string name;
    double price;

    public:

    Product(int sku, string name, double price){
        this->sku=sku;
        this->name=name;
        this->price=price;
    }

    int getSku(){
        return sku;
    }

    string getName(){
        return name;
    }

    double getPrice(){
        return price;
    }
};

class ProductFactory{
    public:
    
    static Product *createProduct(int sku){

        string name;
        double price;

        if (sku == 101) {
            name  = "Apple";
            price = 20;
        }
        else if (sku == 102) {
            name  = "Banana";
            price = 10;
        }
        else if (sku == 103) {
            name  = "Chocolate";
            price = 50;
        }
        else if (sku == 201) {
            name  = "T-Shirt";
            price = 500;
        }
        else if (sku == 202) {
            name  = "Jeans";
            price = 1000;
        }
        else {
            name  = "Item" + to_string(sku);
            price = 100;
        }
        return new Product(sku, name, price);
    }
};

class InventoryStore{
    public:
    virtual ~InventoryStore(){}
    virtual void addProduct(Product *p, int qty)=0;
    virtual void removeProduct(int sku, int qty)=0;
    virtual int checkstock(int sku)=0;
    virtual vector<Product*>listAllProducts()=0;
};


class DBInventoryStore:public InventoryStore{
    private:
    //It is actually stored in database but for as now we are saving it in map
    //sku->qty
    map<int,int>stock;
    //sku->product
    map<int,Product*>products;

    public:

    DBInventoryStore() {}
    ~DBInventoryStore() {}

    void addProduct(Product *prod, int qty) override{
        int sku=prod->getSku();
        if(products.count(sku)==0){
            //Product does not exist
            products[sku]=prod;
        }else{
            delete prod;
        }
        stock[sku]+=qty;
    }

    void removeProduct(int sku, int qty) override{
        if(products.count(sku)==0){
            cout<<"Product with this"+to_string(sku)+"does not exist";
            return;
        }

        int currentqty=stock[sku];
        int remqty=currentqty-qty;
        if(remqty>0){
            stock[sku]=remqty;
        }
        else{
            stock.erase(sku);
        }
    }

    int checkstock(int sku) override{
        if(stock.count(sku)==0){
            return 0;
        }

        return stock[sku];
    }

    vector<Product*>listAllProducts() override{
        vector<Product*>res;
        for(auto it: stock){
           int sku=it.first;
           int qty=it.second;
           if(qty>0 && products.count(sku)){
                res.push_back(products[sku]);
           }
        }
        return res;
    }
};

class InventoryManager{
    private:
    InventoryStore *store;

    public:
    InventoryManager(InventoryStore *store){
       this->store=store;
    }

    void addToStock(int sku, int qty){
        Product *prod=ProductFactory::createProduct(sku);
        store->addProduct(prod,qty);
        cout << "[InventoryManager] Added SKU " << sku << " Qty " << qty << endl;
    }

    void removeStock(int sku, int qty){
        store->removeProduct(sku,qty);
    }

    int checkstock(int sku){
        return store->checkstock(sku);
    }

    vector<Product*>getAvailableProduct(){
        return store->listAllProducts();
    }

};

//Repleshing Strategy

class ReplenishStrategy{
    public:
    //sku->qty
    virtual void replenish(InventoryManager *mgr, map<int,int>replenishItem)=0;
    virtual ~ReplenishStrategy(){}
};

class ThresholdReplenishStrategy:public ReplenishStrategy{
    private:
    int threshold;

    public:

    ThresholdReplenishStrategy(int thres){
        threshold=thres;
    }

    void replenish(InventoryManager *mgr, map<int,int>repItems) override{
        cout << "[ThresholdReplenish] Checking threshold... \n";
        for(auto it: repItems){
            int sku=it.first;
            int qtyToAdd=it.second;
            int current=mgr->checkstock(sku);
            if(current<threshold){
               mgr->addToStock(sku,qtyToAdd);
               cout << "  -> SKU " << sku << " was " << current 
                     << ", replenished by " << qtyToAdd << endl;
            }
        }
    }
};

class WeekelyReplenishStrategy:public ReplenishStrategy{
    public:
        void replenish(InventoryManager *mgr, map<int,int>repItems) override{
        cout << "[WeeklyReplenish] Weekly replenishment triggered for inventory.\n";
        }
    
};


class DarkStore{
    public:
    string name;
    double x,y;
    InventoryManager *mgr;
    ReplenishStrategy *repstrat;

    public:

    DarkStore(string name, double x, double y){
        this->name=name;
        this->x=x;
        this->y=y;
        
        mgr=new InventoryManager(new DBInventoryStore);
    }
    
    ~DarkStore(){
        delete mgr;
        if(repstrat) delete repstrat;
    }

    double distanceTo(double ux, double uy){
        return sqrt((x-ux)*(x-ux)+ (y-uy)*(y-uy));
    }

    void runReplenish(map<int,int>itemToRep){
        if(repstrat){
          repstrat->replenish(mgr,itemToRep);
        }
    }

    void setReplenishStrategy(ReplenishStrategy *strat){
        repstrat=strat;
    }

    vector<Product *>getAllProducts(){
        return mgr->getAvailableProduct();
    }

    void addToStock(int sku, int qty){
        mgr->addToStock(sku,qty);
    }

    void removeStock(int sku, int qty){
        mgr->removeStock(sku,qty);
    }

    int checkstock(int sku){
        return mgr->checkstock(sku);
    }
    
    string getName(){
        return name;
    }

    double getXCordinate(){
        return x;
    }

    double getYCordinate(){
        return y;
    }

    InventoryManager *getManager(){
        return mgr;
    }
};

class DarkStoreManager{
    private:
    static DarkStoreManager *instance;
    vector<DarkStore*>stores;

    public:

    static DarkStoreManager *getInstace(){
        if(!instance){
            instance=new DarkStoreManager();
        }

        return instance;
    }

    void registerDarkStore(DarkStore *ds){
        stores.push_back(ds);
    }

    vector<DarkStore*>getNearbyStores(double ux, double uy, double maxDist){
        vector<pair<double,DarkStore*>>dstores;
        for(auto it:stores){
            double dist=it->distanceTo(ux,uy);
            if(dist<=maxDist){
                dstores.push_back({dist,it});
            }
        }

        sort(dstores.begin(),dstores.end(), [](auto &a, auto &b){ return a.first<b.first;});

        vector<DarkStore*>ds;
        for(auto &it: dstores){
            ds.push_back(it.second);
        }

        return ds;
    }
};

DarkStoreManager* DarkStoreManager::instance = nullptr;

class Cart{
    private:
    vector<pair<Product*,int>>items;
    public:

    void addItem(int sku, int qty){
       Product *product=ProductFactory::createProduct(sku);
       items.push_back({product,qty});

       cout << "[Cart] Added SKU " << sku << " (" << product->getName() 
             << ") x" << qty << endl;
    }

    double getTotal(){
        double total=0.0;
        for(auto &it: items){
            double amount=it.first->getPrice()*it.second;
            total+=amount;
        }

        return total;
    }

    vector<pair<Product*,int>>getItems(){
        return items;
    }

    ~Cart() {
        for (auto &it : items) {
            delete it.first;
        }
    }
};

class User{
    private:
    string name;
    double ux,uy;
    Cart *cart;

    public:
    User(string n, double x, double y){
        name=n;
        x=ux;
        y=uy;
        cart=new Cart();
    }

    string getName(){
        return name;
    }

    double getXCordinate(){
        return ux;
    }

    double getYCordinate(){
        return uy;
    }

    ~User(){
        delete cart;
    }

    Cart *getCart(){
        return cart;
    }
};

class DeliveryPartner{
    public:
    string name;
    DeliveryPartner(string n){
        name=n;
    }
};

class Order{
    public:
    static int nextId;
    int orderId;
    User *user;
    vector<pair<Product*,int>>items;
    vector<DeliveryPartner*>partners;
    double total;

    Order(User *u){
       user=u;
       total=0.0;
       orderId=nextId++;
    }
};

int Order::nextId=1;

class OrderManager{
    private:
    vector<Order*>orders;
    static OrderManager *instance;

    public:

    static OrderManager *getInstance(){
        if(!instance){
            instance=new OrderManager();
        }

        return instance;
    }

    void placeOrder(User *user, Cart *cart){
       cout << "\n[OrderManager] Placing Order for: " << user->getName() << "\n";

       //product->qty
       vector<pair<Product*,int>>reqItems=cart->getItems();

       vector<DarkStore*>nearbyStores=DarkStoreManager::getInstace()->getNearbyStores(user->getXCordinate(),user->getYCordinate(), 5.0);

       if(nearbyStores.empty()){
        cout<<"No stores available around your region";
        return;
       }

       DarkStore* firstnearbyshop=nearbyStores.front();
       bool allItems=true;
       vector<Product*>itemsInShop=firstnearbyshop->getAllProducts();

       for(auto it: reqItems){
        int sku=it.first->getSku();
        int qty=it.second;
        
        if(firstnearbyshop->checkstock(sku)<qty){
            allItems=false;
            break;
        }
       }

       Order *order=new Order(user);

       if(allItems){
        double totalcost=0.0;
         for(auto it: reqItems){
            int sku=it.first->getSku();
            int qty=it.second;
            int price=it.first->getPrice();
            firstnearbyshop->removeStock(sku,qty);
            totalcost+=(price*qty);
         }
        order->total=totalcost;
        DeliveryPartner *partner=new DeliveryPartner("partner1");
        order->partners.push_back(partner);
        return;
       }
       else{
            cout << "  Splitting order across stores...\n";
            //sku->qty
            map<int,int>allItems;

            for(auto it: reqItems){
                allItems[it.first->getSku()]=it.second;
            }
            int partnerId=1;
            for(DarkStore *store: nearbyStores){
                bool assigned=false;
                vector<int>toErase;
                for(pair<int,int>item: allItems){
                    int sku=item.first;
                    int qtyreq=item.second;
                    int qtyavailable=store->checkstock(sku);
                    if(qtyavailable<=0) continue;

                    int takenqty=min(qtyavailable,qtyreq);
                    store->removeStock(sku,takenqty);
                    order->items.push_back({ProductFactory::createProduct(sku),takenqty});

                    if(qtyreq>takenqty){
                        allItems[sku]=qtyreq-takenqty;
                    }
                    else{
                        toErase.push_back(sku);
                    }
                    assigned=true;
                }

                for(int sku: toErase){
                    allItems.erase(sku);
                }
                
                if(assigned){
                   string pname="Patner"+to_string(partnerId++);
                   DeliveryPartner *partner=new DeliveryPartner(pname);
                   order->partners.push_back(partner);
                   cout << "     Assigned: " << pname << " for " << store->getName() << "\n";
                }
            }

            if(!allItems.empty()){
                cout<<"Could not fulfill:\n";
                for(auto&it:allItems){
                    cout << "    SKU " << it.first << " x" << it.second << "\n";
                }
            }

            double sum=0.0;
            for(auto it:order->items){
                sum+=(it.first->getPrice()*it.second);
            }

            order->total=sum;

            orders.push_back(order);

        // Printing Order Summary
        cout << "\n[OrderManager] Order #" << order->orderId << " Summary:\n";
        cout << "  User: " << user->getName() << "\n  Items:\n";
        for (auto& item : order->items) {
            cout << "    SKU " << item.first->getSku()
                 << " (" << item.first->getName() << ") x" << item.second
                 << " @ ₹" << item.first->getPrice() << "\n";
        }
        cout << "  Total: ₹" << order->total << "\n  Partners:\n";
        for (auto* dp : order->partners) {
            cout << "    " << dp->name << "\n";
        }
        cout << endl;

       }
         // Cleanups
        for (auto* dp : order->partners) delete dp;
        for (auto& item : order->items) delete item.first;
    }
    
    vector<Order*> getAllOrders(){
        return orders;
    }

    ~OrderManager(){
        for(auto order:orders){
            delete order;
        }
    }
};

OrderManager *OrderManager::instance=nullptr;


class Zepto{
    public:

    static void showAllProducts(User *user){
     cout << "\n[Zepto] All Available products within 5 KM for " << user->getName() << ":\n";

     vector<DarkStore*>nearbyStores=DarkStoreManager::getInstace()->getNearbyStores(user->getXCordinate(),user->getYCordinate(), 5.0);

     cout<<"Showing shops within 5km of users location"<<endl;
     for(int i=0; i<nearbyStores.size(); i++){
        cout<<to_string(i+1)<<":"<<nearbyStores[i]->getName()<<endl;
     }

     //sku-qty
     map<int,double>skuToPrice;
     //sku-name
     map<int,string>skuToName;

     for(DarkStore *shops: nearbyStores){
          vector<Product*>allProducts=shops->getAllProducts();
          for(Product* prod: allProducts){
            int sku=prod->getSku();
            skuToPrice[sku]=prod->getPrice();
            skuToName[sku]=prod->getName();
          }
     }

     for (auto& entry : skuToPrice) {
            int sku = entry.first;
            double price = entry.second;
            cout << "  SKU " << sku << " - " << skuToName[sku] << " @ ₹" << price << "\n";
        }
    } 

    static void initialize() {
        auto dsManager = DarkStoreManager::getInstace();

        // DarkStore A.......
        DarkStore* darkStoreA = new DarkStore("DarkStoreA", 0.0, 0.0);
        darkStoreA->setReplenishStrategy(new ThresholdReplenishStrategy(3));
 
        cout << "\nAdding stocks in DarkStoreA...." << endl;  
        darkStoreA->addToStock(101, 5); // Apple
        darkStoreA->addToStock(102, 2); // Banana

        // DarkStore B.......
        DarkStore* darkStoreB = new DarkStore("DarkStoreB", 4.0, 1.0);
        darkStoreB->setReplenishStrategy(new ThresholdReplenishStrategy(3));

        cout << "\nAdding stocks in DarkStoreB...." << endl; 
        darkStoreB->addToStock(101, 3); // Apple
        darkStoreB->addToStock(103, 10); // Chocolate

        // DarkStore C.......
        DarkStore* darkStoreC = new DarkStore("DarkStoreC", 2.0, 3.0);
        darkStoreC->setReplenishStrategy(new ThresholdReplenishStrategy(3));

        cout << "\nAdding stocks in DarkStoreC...." << endl; 
        darkStoreC->addToStock(102, 5); // Banana
        darkStoreC->addToStock(201, 7); // T-Shirt

        dsManager->registerDarkStore(darkStoreA);
        dsManager->registerDarkStore(darkStoreB);
        dsManager->registerDarkStore(darkStoreC);
    }
};


int main(){
     // 1) Initialize.
    Zepto::initialize();

    // 2) A User comes on Platform
    User* user = new User("Aditya", 1.0, 1.0);
    cout <<"\nUser with name " << user->getName()<< " comes on platform" << endl;

    // 3) Show all available items via Zepto
    Zepto::showAllProducts(user);

    // 4) User adds items to cart (some not in a single store)
    cout<<"\nAdding items to cart\n";
    Cart* cart = user->getCart();
    cart->addItem(101, 4);  // dsA has 5, dsB has 3 
    cart->addItem(102, 3);  // dsA has 2, dsC has 5
    cart->addItem(103, 2);  // dsB has 10

    // 5) Place Order
    OrderManager::getInstance()->placeOrder(user, user->getCart());

    // 6) Cleanup
    delete user;
    delete DarkStoreManager::getInstace();  // deletes all DarkStores and their inventoryManagers

    return 0;
}



