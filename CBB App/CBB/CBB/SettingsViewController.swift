//
//  SettingsViewController.swift
//  CBB
//
//  Created by Alex Ku on 4/11/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import Foundation
import UIKit

class SettingsViewController: UITableViewController {
    @IBOutlet weak var menuButton: UIBarButtonItem!
    var user_name = ""
    var user_id = ""
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = "Settings"
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func setupMenu() {
        if (revealViewController() != nil) {
            menuButton.target = revealViewController()
            menuButton.action = #selector(SWRevealViewController.revealToggle(_:))
            revealViewController().rearViewRevealWidth = 275
            
            view.addGestureRecognizer(self.revealViewController().panGestureRecognizer())
        }
        customNavBar()
    }
    
    func customNavBar() {
        let navImg = #imageLiteral(resourceName: "nav-bar")
        
        self.navigationController?.navigationBar.setBackgroundImage(navImg, for: .default)
        
        navigationController?.navigationBar.tintColor = UIColor(colorLiteralRed: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
        navigationController?.navigationBar.titleTextAttributes = [NSForegroundColorAttributeName: UIColor.white, NSFontAttributeName: UIFont(name: "Aleo-Bold", size: 20.0)]

    }
    
    
}

