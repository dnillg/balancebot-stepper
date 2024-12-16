package com.dnillg.balancer.controlapp

import dagger.Component

@Component(modules = [MainModule::class])
interface AppComponent {

    fun inject(activity: MainActivity);

}