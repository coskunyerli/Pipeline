#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <iostream>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_actor(nullptr)
{
    // TODOJ Burada circle check yapılmalı
    // Foreach actor, map actor, filter actor, reduce actor yazılmalı
    // Memory manager gibi bir sistem kurulabilir, şimidilk kullanıcı kendisi geliştirsin bunu
    // Buna bir tane Graph editor yazılmalı
    // Her module için bir behaviour override yapılabilir
    //
    auto *mainWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(mainWidget);
    auto *pushButton = new QPushButton("Start", mainWidget);

    mainLayout->addWidget(pushButton);
    auto threadPool = new QThreadPool(this);
    threadPool->setMaxThreadCount(1);

    m_actor = new Pipeline::Actor(threadPool, this);
    m_actor->setBehaviour(Pipeline::Behaviour([](const Pipeline::BehaviourContext& context){
        size_t total = 0;
        std::cout << "root actor started " << total << std::endl;
        for(size_t i = 0;i < 1000; i++)
        {
            total+=i;
        }
        std::cout << "root actor finished " << total << std::endl;
        return QVariant(total);
    }));

    auto *childActor = new Pipeline::Actor(threadPool, this);
    childActor->setBehaviour(Pipeline::Behaviour([](const Pipeline::BehaviourContext& context){

        size_t total = context.m_variants[0].toLongLong();
        std::cout << "child1 actor started " << total << std::endl;
        for(size_t i = 0; i < 1000; i++)
        {
            total+=i;
        }
        std::cout << "child1 actor finished " << total << std::endl;
        return QVariant(total);
    }));

    auto *childActor2 = new Pipeline::Actor(threadPool, this);
    childActor2->setBehaviour(Pipeline::Behaviour([](const Pipeline::BehaviourContext& context){
        size_t total = context.m_variants[0].toLongLong();
        std::cout << "child2 actor started " << total << std::endl;
        for(size_t i = 0;i < 1000;i++)
        {
            total+=i;
        }
        std::cout << "child2 actor finished " << total << std::endl;
        return QVariant(total);
    }));

    auto *childActor3 = new Pipeline::Actor(threadPool, this);
    childActor3->setBehaviour(Pipeline::Behaviour([](const Pipeline::BehaviourContext& context){
        size_t total = context.m_variants[0].toLongLong() + context.m_variants[1].toLongLong();
        std::cout << "child3 actor started " << total << std::endl;
        for(size_t i = 0; i < 1000;i++)
        {
            total+=i;
        }
        std::cout << "child3 actor finished " << total << std::endl;
        return QVariant(total);
    }));

    m_actor->addNextActor(childActor);
    m_actor->addNextActor(childActor2);

    childActor->addNextActor(childActor3);
    childActor2->addNextActor(childActor3);

    connect(pushButton,&QPushButton::clicked, this, [this](){
        m_actor->reset();
        m_actor->startRequest();
    });
    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow() {}
