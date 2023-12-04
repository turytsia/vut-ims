#include "simlib.h"
#include <vector>
#include <algorithm>

#define GROUP_SIZE 32
Store Pool("Pool capacity", 634);
Store Booths("Booths", 8);
Facility Reception("Reception");
Queue boothQueue("Booth queue");
// Queue profSwimmers("Proffessional swimmers");
int MorningGroups = 2;
int AfternoonNoGroups = 6;
int EveningGroups = 5;
int people_got_in = 0;
Histogram Table("Table", 0, 25, 20);

class Visitor : public Process
{
private:
    double Entry;
    bool isGroup;
    bool skipBooth = false;
    int interrupted = 0;

public:
    Visitor(bool isGroup = false);

    void Behavior() override;

    void setSkipBooth();
    void interrupt();
};

class SwimManager
{
private:
    std::vector<Visitor *> swimmerList;

public:
    void addSwimmer(Visitor *swimmer)
    {
        swimmerList.push_back(swimmer);
    }

    bool isSwimmerInList(const Visitor *swimmer) const
    {
        return std::find(swimmerList.begin(), swimmerList.end(), swimmer) != swimmerList.end();
    }

    void removeSwimmer(const Visitor *swimmer)
    {
        auto it = std::remove(swimmerList.begin(), swimmerList.end(), swimmer);
        swimmerList.erase(it, swimmerList.end());
    }

    size_t getSwimmerCount() const
    {
        return swimmerList.size();
    }

    std::vector<Visitor *>::const_iterator begin() const
    {
        return swimmerList.begin();
    }

    std::vector<Visitor *>::const_iterator end() const
    {
        return swimmerList.end();
    }
};

SwimManager swimManager;

class VisitorWantsBooth : public Process
{
private:
    Visitor *visitor;

public:
    VisitorWantsBooth(Visitor *visitor) : visitor(visitor) {}

    void Behavior()
    {
        printf("vwb: start to wait for booth\n");
        Wait(120);
        visitor->setSkipBooth();
        printf("vwb: want to activate a visitor\n");
        visitor->Activate();
        printf("vwb: activated a visitor\n");
    }
};

Visitor::Visitor(bool isGroup) : isGroup(isGroup) {}

void Visitor::Behavior()
{
    Entry = Time;
    if (!isGroup)
    {
        if (Pool.Free() >= 1)
        {
            Enter(Pool, 1);
            people_got_in++;
            printf("Entered pool\n");
        }
        else
        {
            Cancel();
        }

        if (Random() < 0.30)
        {
            Seize(Reception);
            Wait(Exponential(180));
            Release(Reception);
        }
    }
    //////////////////////
    // in changing room //
    //////////////////////
    if (Random() < 0.40) // new inBooth -> activate{ Wait (120)} ; into Queue
    {
        printf("i want a booth\n");
        if (Booths.Full()) // no free booth
        {
            printf("FULL booths\n");
            Into(boothQueue);
            VisitorWantsBooth *vwb = new VisitorWantsBooth(this);
            vwb->Activate();
            printf("before passivate\n");
            Passivate();
            if (!skipBooth)
            {
                printf("dockal som sa\n");
                vwb->Cancel();
            }
        }
        if (!skipBooth)
        {
            printf("not skipping booth\n");
            Enter(Booths, 1);
            printf("entered booth\n");
            Wait(Normal(240, 60));
            printf("undressed in booth\n");
            Leave(Booths, 1);
            printf("used booth\n");
            while (boothQueue.Length() > 0)
            {
                Visitor *v = (Visitor *)boothQueue.GetFirst();
                if (!(v->skipBooth))
                { // old ones, which decided to leave the queue, are skipped
                    printf("SOMEBODY IN\n");
                    v->Activate();
                    printf("ACTIVATED\n");
                    break;
                }
                printf("somebody left\n");
            }
        }
    }
    else
    {
        printf("i do not want a booth\n");
        skipBooth = true;
    }
    if (skipBooth)
    {
        printf("skipping booth\n");
        Wait(Normal(180, 60));
        printf("undressed in hall\n");
    }
    skipBooth = false;
    ////////////////
    // near pools //
    ////////////////

    if (isGroup)
    {
        printf("group swimming\n");
        Wait(3600);
        printf("group done\n");
    }
    else
    {
        printf("WE WER\n");
        double swimmerType = Random();
        if (swimmerType < 0.05)
        {
            printf("babushka\n");
            // unsigned int length = profSwimmers.Length();
            // for (unsigned int i = 0; i < length; i++)
            // {
            //     Visitor *v = (Visitor *)profSwimmers.GetFirst();
            //     v->interrupt();
            //     // profSwimmers.Insert(v);
            //     swimManager.addSwimmer(v);
            //     printf("interrupted!!!!\n");
            //     printf("pro, length of queue: %d\n", swimManager.getSwimmerCount());
            // }
            for (const auto swimmer : swimManager)
            {
                swimmer->interrupt();
            }
            goto normal;
        }
        else if (swimmerType < 0.15) // 10%
        {
            printf("pro, length of queue: %ld\n", swimManager.getSwimmerCount());
            swimManager.addSwimmer(this);
            // profSwimmers.Insert(this);
            printf("in list to interrupt\n");
            Wait(Normal(3600, 600));
            printf("pro swum\n");
            // Visitor *v = (Visitor *)profSwimmers.GetFirst();
            // while (v != this)
            // {
            //     profSwimmers.Insert(v);
            //     v = (Visitor *)profSwimmers.GetFirst();
            // }
            swimManager.removeSwimmer(this);
        }
        else
        {
        normal:
            printf("normal\n");
            Wait(Normal(2400, 600));
            Wait(Normal(1800, 300));
        }
        printf("WE WERE HERE\n");
    }
    //////////
    // swum //
    //////////
    Wait(Exponential(300));
    printf("showered after pool\n");

    // in changing room
    if (Random() < 0.40) // new inBooth -> activate{ Wait (120)} ; into Queue
    {
        printf("i want a booth (AFTER)\n");
        if (Booths.Full()) // no free booth
        {
            printf("FULL booths\n");
            Into(boothQueue);
            VisitorWantsBooth *vwb = new VisitorWantsBooth(this);
            vwb->Activate();
            printf("before passivate\n");
            Passivate();
            if (!skipBooth)
            {
                printf("dockal som sa after\n");
                vwb->Cancel();
                printf("cancelled timer\n");
            }
        }
        if (!skipBooth)
        {
            printf("not skipping booth\n");
            Enter(Booths, 1);
            printf("entered booth\n");
            Wait(Normal(360, 60));
            printf("undressed in booth\n");
            Leave(Booths, 1);
            printf("used booth\n");
            while (boothQueue.Length() > 0)
            {
                Visitor *v = (Visitor *)boothQueue.GetFirst();
                if (!(v->skipBooth))
                { // old ones, which decided to leave the queue, are skipped
                    printf("SOMEBODY IN\n");
                    v->Activate();
                    printf("ACTIVATED\n");
                    break;
                }
                printf("somebody left\n");
            }
        }
    }
    else
    {
        printf("i do not want a booth\n");
        skipBooth = true;
    }
    if (skipBooth)
    {
        printf("skipping booth\n");
        Wait(Normal(300, 60));
        printf("undressed in hall\n");
    }
    Leave(Pool, 1);

    Table(Time - Entry);
}

void Visitor::setSkipBooth()
{
    skipBooth = true;
}

void Visitor::interrupt()
{
    interrupted++;
}

// class Visitor : public Process
// {
// private:
//     double Entry;
//     bool isGroup;
//     bool skipBooth = false;
//     int interrupted = 0;

// public:
//     Visitor(bool isGroup = false) : isGroup(isGroup) {}

//     void Behavior()
//     {
//         Entry = Time;
//         if (!isGroup)
//         {
//             if (Pool.Free() >= 1)
//             {
//                 Enter(Pool, 1);
//             }
//             else
//             {
//                 Cancel();
//             }

//             if (Random() < 0.30)
//             {
//                 Seize(Reception);
//                 Wait(Exponential(180));
//                 Release(Reception);
//             }
//         }
//         // in changing room
//         if (Random() < 0.40) // new inBooth -> activate{ Wait (120)} ; into Queue
//         {
//             if (Booths.Full()) // no free booth
//             {
//                 Into(boothQueue);
//                 (new VisitorWantsBooth(this))->Activate();
//                 Passivate();
//             }
//             if (!skipBooth)
//             {
//                 Enter(Booths, 1);
//                 Wait(Normal(240, 60));
//                 Leave(Booths, 1);
//                 if (boothQueue.Length() > 0)
//                 {
//                     boothQueue.GetFirst()->Activate();
//                 }
//             }
//         }
//         else
//         {
//             skipBooth = true;
//         }
//         if (skipBooth)
//         {
//             Wait(Normal(180, 60));
//         }

//         if (isGroup)
//         {
//             Wait(3600);
//         }
//         else
//         {
//             double swimmerType = Random();
//             if (swimmerType < 0.05)
//             {
//                 unsigned int length = profSwimmers.Length();
//                 for (unsigned int i = 0; i < length; i++)
//                 {
//                     Visitor *v = (Visitor *)profSwimmers.GetFirst();
//                     v->interrupt();
//                     profSwimmers.Insert(v);
//                 }
//                 goto normal;
//             }
//             else if (swimmerType < 0.15) // 10%
//             {
//                 profSwimmers.Insert(this);
//                 Wait(Normal(3600, 600));
//                 Visitor *v = (Visitor *)profSwimmers.GetFirst();
//                 while (v != this)
//                 {
//                     profSwimmers.Insert(v);
//                     v = (Visitor *)profSwimmers.GetFirst();
//                 }
//             }
//             else
//             {
//             normal:
//                 Wait(Normal(2400, 600));
//                 Wait(Normal(1800, 300));
//             }
//         }
//         Table(Time - Entry);
//     }

//     void setSkipBooth()
//     {
//         skipBooth = true;
//     }

//     void interrupt()
//     {
//         interrupted++;
//     }
// };

class Group : public Process
{
    void Behavior()
    {
        if (Pool.Free() >= GROUP_SIZE)
        {
            Enter(Pool, GROUP_SIZE);
            for (int i = 0; i < GROUP_SIZE; i++)
            {
                (new Visitor(true))->Activate();
                people_got_in++;
            }
        }
    }
};

class Hour : public Process
{
    void Behavior()
    {
        if (MorningGroups > 0)
        {
            MorningGroups--;
            Wait(3600);
            (new Group)->Activate();
            printf("Morning group arrived\n");
            (new Hour)->Activate();
        }
        else if (AfternoonNoGroups > 0)
        {
            AfternoonNoGroups--;
            Wait(3600);
            printf("Afternoon without group\n");
            (new Hour)->Activate();
        }
        else if (EveningGroups > 0)
        {
            EveningGroups--;
            Wait(3600);
            (new Group)->Activate();
            printf("Evening group arrived\n");
            (new Hour)->Activate();
        }
    }
};

class VisitorGenerator : public Event
{
    void Behavior()
    {
        (new Visitor(false))->Activate();
        double d = Exponential(60);
        Activate(Time + d);
    }
};

int main()
{
    Print(" pool - SIMLIB/C++ example\n");
    SetOutput("pool.out");
    Init(0, 54000); // 6-21
    (new VisitorGenerator)->Activate();
    (new Hour)->Activate();
    (new Group)->Activate();
    Run();
    Pool.Output();
    Table.Output();
    printf("people got in: %d\n", people_got_in);
    return 0;
}
