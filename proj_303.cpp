#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;
class BTCNode : public cSimpleModule
{
  private:
    cMessage *event1;
    cMessage *event2;

  public:
     int n;
     int s;
      int counter1; //I have the Block
      int counter2; //I sent fast the Block
      int counter3; //I sent the slow block
      float Block_Processing_Time=0.016; //Block processing time consists of block reconstruction+processing+block verification
      int m=3;
      BTCNode();
            virtual ~BTCNode();

          protected:
            virtual void initialize() override;
            virtual void handleMessage(cMessage *msg) override;
        };

Define_Module(BTCNode);

BTCNode::BTCNode()
{

    event1= nullptr;
    event2= nullptr;

}

BTCNode::~BTCNode()
{

    cancelAndDelete(event1);
    cancelAndDelete(event2);

}


void BTCNode::initialize()
{
    counter1 = 0;
    WATCH(counter1);
    counter2=0;
    WATCH(counter2);
    counter3 = 0;
    WATCH(counter3);


    event2 = new cMessage("event2");

    if (getIndex()==0) { //start from initiator node


        scheduleAt(0.0, event2);
                       }
}



void BTCNode::handleMessage(cMessage *msg)
  {

    if (strcmp("event2", msg->getName()) == 0) {


        if(counter2!=0){ //means I have already sent the fast block
      //
            cancelAndDelete(msg);
        //cancelEvent(event2);
        //delete msg;
        event1 = new cMessage("event1");
         scheduleAt(simTime()+Block_Processing_Time, event1);
                        }

        else{
            cPacket *pk = new cPacket("cmpctBlock");
                    pk->setByteLength(3600);//---------------------------------------------------------------------->
                //    n = gateSize("gate");
                    for (int i = 0; i < m; i++)

                       // if (i<3 )
                                   {
                                        cPacket *copy = pk->dup();

                                        send(copy, "gate$o", i);

                                   }
                           delete pk;
                           counter2++;
                           cancelAndDelete(msg);
                           event1 = new cMessage("event1");
                           scheduleAt(simTime()+Block_Processing_Time, event1);

             }

   }



else if (strcmp("event1", msg->getName()) == 0) {


        if(counter3!=0){
                      delete msg;
                        }

        else{
        cPacket *pk = new cPacket("inv");
        pk->setByteLength(37);
       // EV << "The scenario is started, sending inv message to the neighboring nodes. \n";
        s = gateSize("gate");
        for (int i =m; i < s-1; i++)
                       {
                            cPacket *copy = pk->dup();

                           send(copy, "gate$o", i);

                       }
               delete pk;
               counter3++;

    }}

    //****************

else if (strcmp("inv", msg->getName()) == 0){
                                                    if(counter1!=0){
                                                        delete msg;

                                               //        EV << "Duplicate inv message- I have this block! \n";
                                                     }
                                                     else {

        int U1= msg->getArrivalGate()->getIndex();
        cPacket *pk = new cPacket("GetData");
        pk->setByteLength(37);

                   send(pk, "gate$o",U1);
                   //counter++;
                        }
    }
    //*****************


else if(strcmp("GetData", msg->getName()) == 0)
                     {
                       //   if(counter1!=0){
                         // delete msg;
                       //     }
                      //    else{
                     int U2= msg->getArrivalGate()->getIndex();
                     cPacket *pk = new cPacket("cmpctBlock");
                     pk->setByteLength(3600);  //------------------------------------------------------------->
                    send(pk, "gate$o",U2);

                 //    }
                     }




else if(strcmp("cmpctBlock", msg->getName()) == 0){



       event2 = new cMessage("event2");
      scheduleAt(simTime()+0.0, event2);
      counter1++;
   }



}








