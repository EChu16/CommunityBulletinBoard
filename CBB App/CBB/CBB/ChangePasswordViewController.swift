//
//  ChangePasswordViewController.swift
//  CBB
//
//  Created by Alex Ku on 4/11/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import Foundation
import UIKit

class ChangePasswordViewController: UITableViewController {
    
    @IBOutlet weak var oldPasswordField: UITextField!
    @IBOutlet weak var retypeOldPasswordField: UITextField!
    @IBOutlet weak var newPasswordField: UITextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    
}
