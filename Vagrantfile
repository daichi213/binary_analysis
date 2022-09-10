# -*- mode: ruby -*-
# vi: set ft=ruby :
Vagrant.configure("2") do |config|

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://vagrantcloud.com/search.
  config.vm.box = "base"

  config.vm.define :test do |node|
    config.vm.box = "ubuntu/trusty32"
    node.vm.synced_folder "./work", "/home/vagrant/work"

    node.vm.provision "shell", inline: <<-SHELL
      sudo apt-get update
      sudo apt-get install -y git gdb nasm
      sudo git clone https://github.com/longld/peda.git ~/peda
      sudo echo "source ~/peda/peda.py" >> ~/.gdbinit
    SHELL
  end
end
